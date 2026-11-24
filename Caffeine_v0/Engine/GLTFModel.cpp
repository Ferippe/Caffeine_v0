#include "GLTFModel.h"
#include "ShaderUtils.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

// UNICO translation unit que implementa tinygltf y stb_image.
// No definir estos macros de nuevo en ningun otro .cpp.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

// Lee los componentes float de un atributo glTF (respeta byteOffset y byteStride).
static std::vector<float> leerAtributo(const tinygltf::Model& model,
    const tinygltf::Primitive& prim, const std::string& nombre, int componentes)
{
    auto it = prim.attributes.find(nombre);
    if (it == prim.attributes.end()) return {};

    const auto& acc = model.accessors[it->second];
    if (acc.bufferView < 0 || acc.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) return {};
    const auto& bv = model.bufferViews[acc.bufferView];
    const auto& buf = model.buffers[bv.buffer];
    int stride = acc.ByteStride(bv);
    if (stride <= 0) stride = componentes * sizeof(float);

    std::vector<float> datos(acc.count * componentes);
    const unsigned char* base = buf.data.data() + bv.byteOffset + acc.byteOffset;
    for (size_t i = 0; i < acc.count; i++)
        std::memcpy(datos.data() + i * componentes, base + i * stride, componentes * sizeof(float));
    return datos;
}

static std::vector<unsigned int> leerIndices(const tinygltf::Model& model,
    const tinygltf::Primitive& prim, size_t vertexCount)
{
    std::vector<unsigned int> indices;
    if (prim.indices < 0) {
        indices.resize(vertexCount);
        std::iota(indices.begin(), indices.end(), 0u);
        return indices;
    }
    const tinygltf::Accessor& acc = model.accessors[prim.indices];
    const tinygltf::BufferView& bv = model.bufferViews[acc.bufferView];
    const tinygltf::Buffer& buf = model.buffers[bv.buffer];
    const unsigned char* base = buf.data.data() + bv.byteOffset + acc.byteOffset;

    int compSize = tinygltf::GetComponentSizeInBytes(acc.componentType);
    int stride = acc.ByteStride(bv);
    if (stride <= 0) stride = compSize;

    indices.resize(acc.count);
    for (size_t i = 0; i < acc.count; i++) {
        const unsigned char* p = base + i * stride;
        switch (acc.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  indices[i] = *p; break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: indices[i] = *reinterpret_cast<const unsigned short*>(p); break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:   indices[i] = *reinterpret_cast<const unsigned int*>(p); break;
        default: indices[i] = 0; break;
        }
    }
    return indices;
}

static glm::mat4 matrizDeNodo(const tinygltf::Node& node)
{
    if (node.matrix.size() == 16) {
        glm::mat4 m;
        for (int i = 0; i < 16; i++) glm::value_ptr(m)[i] = (float)node.matrix[i];
        return m;
    }
    glm::mat4 t(1.0f), r(1.0f), s(1.0f);
    if (node.translation.size() == 3)
        t = glm::translate(glm::mat4(1.0f), glm::vec3((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]));
    if (node.rotation.size() == 4)  // glTF guarda xyzw; glm::quat espera wxyz
        r = glm::mat4_cast(glm::quat((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]));
    if (node.scale.size() == 3)
        s = glm::scale(glm::mat4(1.0f), glm::vec3((float)node.scale[0], (float)node.scale[1], (float)node.scale[2]));
    return t * r * s;
}

static void crearMalla(const tinygltf::Model& model, const tinygltf::Primitive& prim,
    const glm::mat4& nodeMatrix, RenderModel& rm)
{
    if (prim.mode != TINYGLTF_MODE_TRIANGLES) return;

    const auto pos = leerAtributo(model, prim, "POSITION", 3);
    if (pos.empty()) return;
    const auto nor = leerAtributo(model, prim, "NORMAL", 3);
    const auto uv = leerAtributo(model, prim, "TEXCOORD_0", 2);
    size_t nPos = pos.size() / 3;

    // Cada vertice contiene 8 floats: posicion (3), normal (3), UV (2).
    // Tambien calculamos la caja contenedora en coordenadas del mundo.
    std::vector<float> vertices(nPos * 8);
    for (size_t i = 0; i < nPos; i++) {
        float* vertice = vertices.data() + i * 8;
        const float* posicion = pos.data() + i * 3;
        std::copy_n(posicion, 3, vertice);
        vertice[4] = 1.0f; // Normal por defecto: (0, 1, 0).
        if (i < nor.size() / 3) std::copy_n(nor.data() + i * 3, 3, vertice + 3);
        if (i < uv.size() / 2) std::copy_n(uv.data() + i * 2, 2, vertice + 6);

        glm::vec3 w = glm::vec3(nodeMatrix * glm::vec4(posicion[0], posicion[1], posicion[2], 1.0f));
        rm.bmin = glm::min(rm.bmin, w);
        rm.bmax = glm::max(rm.bmax, w);
    }

    const auto indices = leerIndices(model, prim, nPos);

    MeshData md;
    md.modelMatrix = nodeMatrix;
    md.indexCount = (int)indices.size();

    // Material: textura base y color base
    if (prim.material >= 0) {
        const auto& mat = model.materials[prim.material];
        const auto& pbr = mat.pbrMetallicRoughness;
        if (pbr.baseColorFactor.size() == 4)
            md.color = glm::vec4((float)pbr.baseColorFactor[0], (float)pbr.baseColorFactor[1],
                (float)pbr.baseColorFactor[2], (float)pbr.baseColorFactor[3]);
        int texIdx = pbr.baseColorTexture.index;
        if (texIdx >= 0 && texIdx < (int)rm.textures.size()) md.texture = rm.textures[texIdx];
    }

    glGenVertexArrays(1, &md.vao);
    glGenBuffers(1, &md.vbo);
    glGenBuffers(1, &md.ibo);

    glBindVertexArray(md.vao);
    glBindBuffer(GL_ARRAY_BUFFER, md.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    rm.meshes.push_back(md);
}

static void procesarNodo(const tinygltf::Model& model, int nodeIdx,
    const glm::mat4& parent, RenderModel& rm)
{
    const tinygltf::Node& node = model.nodes[nodeIdx];
    glm::mat4 world = parent * matrizDeNodo(node);

    if (node.mesh >= 0) {
        for (const auto& prim : model.meshes[node.mesh].primitives)
            crearMalla(model, prim, world, rm);
    }
    for (int child : node.children) procesarNodo(model, child, world, rm);
}

// ---------------------------------------------------------------- Carga GLTF
RenderModel cargarGLTF(const std::string& filepath)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool res = false;
    if (filepath.rfind(".glb") != std::string::npos)
        res = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    else
        res = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);

    if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
    if (!err.empty())  std::cout << "ERR: " << err << std::endl;
    if (!res) {
        std::cout << "Fallo al cargar el archivo GLTF: " << filepath << std::endl;
        return {};
    }

    RenderModel rm;

    // 1. Texturas
    rm.textures.resize(model.textures.size(), 0);
    for (size_t i = 0; i < model.textures.size(); i++) {
        const auto& tex = model.textures[i];
        if (tex.source < 0) continue;
        const auto& img = model.images[tex.source];

        GLenum format = GL_RGBA;
        if (img.component == 3) format = GL_RGB;
        else if (img.component == 1) format = GL_RED;

        rm.textures[i] = crearTextura2D(img.width, img.height, format, format,
                                        GL_REPEAT, img.image.data());
    }

    // 2. Mallas: recorremos la escena para aplicar las transformaciones de cada nodo
    int sceneIdx = model.defaultScene >= 0 ? model.defaultScene : 0;
    if (!model.scenes.empty()) {
        for (int n : model.scenes[sceneIdx].nodes)
            procesarNodo(model, n, glm::mat4(1.0f), rm);
    }

    std::cout << "Mallas cargadas: " << rm.meshes.size() << std::endl;
    return rm;
}

void liberarGLTF(RenderModel& rm) {
    for (auto& m : rm.meshes) {
        if (m.vao) glDeleteVertexArrays(1, &m.vao);
        if (m.vbo) glDeleteBuffers(1, &m.vbo);
        if (m.ibo) glDeleteBuffers(1, &m.ibo);
    }
    for (GLuint& t : rm.textures) {
        if (t) glDeleteTextures(1, &t);
        t = 0;
    }
    rm.meshes.clear();
    rm.textures.clear();
}

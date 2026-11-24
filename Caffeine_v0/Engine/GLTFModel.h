#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

// Tipos compartidos con el renderizador 3D y la camara.
struct MeshData {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    int indexCount = 0;
    GLuint texture = 0;                       // 0 = sin textura
    glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);  // transformacion del nodo
};

struct RenderModel {
    std::vector<MeshData> meshes;
    std::vector<GLuint> textures;
    glm::vec3 bmin = glm::vec3(1e30f);   // caja contenedora del modelo (sin transformacion global)
    glm::vec3 bmax = glm::vec3(-1e30f);
};


// Carga modelos .gltf/.glb, crea texturas y mallas OpenGL.
RenderModel cargarGLTF(const std::string& filepath);
// Libera VAO/VBO/EBO y texturas que cargarGLTF reservo.
void liberarGLTF(RenderModel& rm);

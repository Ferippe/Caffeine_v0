#include "ShaderUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"

namespace {
std::string leerTexto(const std::string& ruta) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo) {
        std::cerr << "ShaderUtils: no se encontro '" << ruta
                  << "'. Revisa el directorio de trabajo de Visual Studio.\n";
        return {};
    }
    std::ostringstream contenido;
    contenido << archivo.rdbuf();
    return contenido.str();
}
}

GLuint compilarShader(GLenum tipo, const char* codigo) {
    GLuint shader = glCreateShader(tipo);
    glShaderSource(shader, 1, &codigo, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "ShaderUtils: error compilando GLSL: " << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint crearProgramaDesdeArchivos(const std::string& vertexPath,
                                const std::string& fragmentPath) {
    const std::string v = leerTexto(vertexPath);
    const std::string f = leerTexto(fragmentPath);
    if (v.empty() || f.empty()) return 0;
    GLuint vs = compilarShader(GL_VERTEX_SHADER, v.c_str());
    GLuint fs = compilarShader(GL_FRAGMENT_SHADER, f.c_str());
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return 0;
    }
    GLuint programa = glCreateProgram();
    glAttachShader(programa, vs);
    glAttachShader(programa, fs);
    glLinkProgram(programa);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = GL_FALSE;
    glGetProgramiv(programa, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048] = {};
        glGetProgramInfoLog(programa, sizeof(log), nullptr, log);
        std::cerr << "ShaderUtils: error enlazando '" << vertexPath
                  << "' / '" << fragmentPath << "': " << log << '\n';
        glDeleteProgram(programa);
        return 0;
    }
    return programa;
}

GLuint crearTextura2D(int ancho, int alto, GLint formatoInterno, GLenum formato,
                      GLint wrap, const void* pixeles) {
    GLuint textura = 0;
    glGenTextures(1, &textura);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textura);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, formatoInterno, ancho, alto, 0, formato, GL_UNSIGNED_BYTE, pixeles);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textura;
}

GLuint cargarPNG(const std::string& ruta, int& ancho, int& alto) {
    unsigned char* pixeles = stbi_load(ruta.c_str(), &ancho, &alto, nullptr, STBI_rgb_alpha);
    if (!pixeles) {
        ancho = alto = 0;
        std::cerr << "No se encontro PNG: " << ruta << '\n';
        return 0;
    }

    GLuint textura = crearTextura2D(ancho, alto, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, pixeles);
    stbi_image_free(pixeles);
    std::cout << "PNG cargado: " << ruta << " (" << ancho << "x" << alto << ")\n";
    return textura;
}

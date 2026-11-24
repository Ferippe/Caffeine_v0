#pragma once
#include <glad/glad.h>
#include <string>

// Compila un shader GLSL desde el texto proporcionado. Devuelve 0 si falla.
GLuint compilarShader(GLenum tipo, const char* codigo);

// Lee dos archivos .vert/.frag, los compila y enlaza el programa OpenGL.
// Las rutas son relativas al DIRECTORIO DE TRABAJO de Visual Studio.
GLuint crearProgramaDesdeArchivos(const std::string& vertexPath,
                                const std::string& fragmentPath);

// Sube pixeles ya decodificados a la GPU y configura filtrado y mipmaps.
GLuint crearTextura2D(int ancho, int alto, GLint formatoInterno, GLenum formato,
                      GLint wrap, const void* pixeles);

// Carga un PNG RGBA en la GPU. Devuelve 0 si falta; ancho y alto se devuelven por referencia.
GLuint cargarPNG(const std::string& ruta, int& ancho, int& alto);

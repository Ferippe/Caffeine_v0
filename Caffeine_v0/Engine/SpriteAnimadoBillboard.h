#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>

// Un billboard 2D animado dentro del espacio 3D.
class SpriteAnimadoBillboard {
public:
    // VARIABLES EDITABLES: las mismas que tenia el main.cpp original.
    float x = 0.0f;            // Posicion horizontal en mundo 3D
    float y = 0.0f;            // Altura de los pies
    float z = 0.0f;            // Profundidad
    float escala = 3.0f;       // Altura de referencia: 672 px en unidades 3D
    float fps = 12.0f;
    bool visible = true;
    float anchoOriginal = 538.0f;
    float altoOriginal = 672.0f;

    SpriteAnimadoBillboard() = default;
    SpriteAnimadoBillboard(const SpriteAnimadoBillboard&) = delete;
    SpriteAnimadoBillboard& operator=(const SpriteAnimadoBillboard&) = delete;
    void init(const std::string& carpeta = "", const std::string& prefijo = "anim");
    // main activa profundidad y mezcla alfa, y desactiva escritura de profundidad.
    void dibujar(float tiempo, const glm::mat4& projection, const glm::mat4& view);
    void liberar();
private:
    static constexpr int NUM_FRAMES = 6;
    GLuint programa = 0, vao = 0;
    GLuint fotogramas[NUM_FRAMES] = {};
};

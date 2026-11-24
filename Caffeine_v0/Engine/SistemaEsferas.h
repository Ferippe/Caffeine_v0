#pragma once
#include <glm/glm.hpp>
#include <random>
#include <glad/glad.h>

// Efectos de luces puntuales y esferas que siguen curvas Bezier.
class SistemaEsferas {
public:
    SistemaEsferas() = default;
    SistemaEsferas(const SistemaEsferas&) = delete;
    SistemaEsferas& operator=(const SistemaEsferas&) = delete;
    void init(const glm::vec3& bmin, const glm::vec3& bmax);
    void actualizar(float dt);
    void vincularLuces(GLuint programaModelo);
    void enviarLuces();
    void dibujar(const glm::mat4& projection, const glm::mat4& view);
    void liberar();
private:
    // Cada luz conserva sus propios puntos de control y generador aleatorio.
    struct EsferaBezier {
        glm::vec3 color;
        float duracionTramo;
        float t = 0.0f;
        glm::vec3 p0, p1, p2, p3, posicion;
        std::mt19937 rng;
        glm::vec3 puntoAleatorio(const glm::vec3& rMin, const glm::vec3& rMax);
        void iniciar(const glm::vec3& rMin, const glm::vec3& rMax);
        void siguienteTramo(const glm::vec3& rMin, const glm::vec3& rMax);
        void actualizar(float dt, const glm::vec3& rMin, const glm::vec3& rMax);
    };
    EsferaBezier esferas[2];
    GLuint programa = 0, vao = 0, vbo = 0, ibo = 0;
    GLint locMVP = -1, locColor = -1;
    GLint locLuzPos = -1, locLuzColor = -1, locLuzRadio = -1, locLuzIntensidad = -1;
    int indexCount = 0;
    float radio = 0.1f, radioLuz = 20.0f, intensidadLuz = 10.0f;
    glm::vec3 regionMin{0.0f}, regionMax{1.0f};
    void crearMalla(int stacks, int sectors);
};

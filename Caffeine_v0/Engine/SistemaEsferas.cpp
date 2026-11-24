#include "SistemaEsferas.h"
#include "ShaderUtils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

// Bezier cubica: B(t) = (1-t)^3 P0 + 3(1-t)^2 t P1 + 3(1-t) t^2 P2 + t^3 P3
static glm::vec3 bezierCubica(const glm::vec3& p0, const glm::vec3& p1,
    const glm::vec3& p2, const glm::vec3& p3, float t)
{
    float u = 1.0f - t;
    return u * u * u * p0 + 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t * p3;
}

glm::vec3 SistemaEsferas::EsferaBezier::puntoAleatorio(const glm::vec3& rMin, const glm::vec3& rMax) {
    std::uniform_real_distribution<float> d(0.0f, 1.0f);
    return glm::vec3(rMin.x + d(rng) * (rMax.x - rMin.x),
        rMin.y + d(rng) * (rMax.y - rMin.y),
        rMin.z + d(rng) * (rMax.z - rMin.z));
}

void SistemaEsferas::EsferaBezier::iniciar(const glm::vec3& rMin, const glm::vec3& rMax) {
    p0 = puntoAleatorio(rMin, rMax);
    p1 = puntoAleatorio(rMin, rMax);
    p2 = puntoAleatorio(rMin, rMax);
    p3 = puntoAleatorio(rMin, rMax);
    t = 0.0f;
    posicion = p0;
}

void SistemaEsferas::EsferaBezier::siguienteTramo(const glm::vec3& rMin, const glm::vec3& rMax) {
    // Reflejar el control anterior y limitarlo a la caja.
    p0 = p3;
    p1 = glm::clamp(p3 + (p3 - p2), rMin, rMax);
    p2 = puntoAleatorio(rMin, rMax);
    p3 = puntoAleatorio(rMin, rMax);
    t = 0.0f;
}

void SistemaEsferas::EsferaBezier::actualizar(float dt, const glm::vec3& rMin, const glm::vec3& rMax) {
    t += dt / duracionTramo;
    while (t >= 1.0f) {
        float sobrante = t - 1.0f;
        siguienteTramo(rMin, rMax);
        t = sobrante;
    }
    posicion = bezierCubica(p0, p1, p2, p3, t);
}

void SistemaEsferas::init(const glm::vec3& bmin, const glm::vec3& bmax) {
    glm::vec3 tam = bmax - bmin;
    glm::vec3 centro = (bmin + bmax) * 0.5f;

    // --- Ajusta estas fracciones para cambiar por donde se mueven ---
    // X: franja central | Y: alturas medias | Z: mitad trasera (zona del escenario)
    regionMin = glm::vec3(centro.x - tam.x * 0.30f, bmin.y + tam.y * 0.20f, bmin.z + tam.z * 0.10f);
    regionMax = glm::vec3(centro.x + tam.x * 0.30f, bmin.y + tam.y * 0.65f, bmin.z + tam.z * 0.55f);
    radio = glm::length(tam) * 0.012f;
    radioLuz = glm::length(tam) * 0.55f;   // alcance de la luz (sube/baja este factor)
    intensidadLuz = 5.0f;                  // intensidad de la luz de color

    // Shader propio
    programa = crearProgramaDesdeArchivos("Shaders/Esferas.vert", "Shaders/Esferas.frag");
    // Aunque el shader falle, generar las posiciones de las luces permite
    // que la escena siga funcionando sin dibujar las esferas.
    if (programa) {
        locMVP = glGetUniformLocation(programa, "uMVP");
        locColor = glGetUniformLocation(programa, "uColor");
    }

    crearMalla(24, 48);

    // Esfera 1: azul celeste | Esfera 2: violeta (distinta velocidad y semilla)
    esferas[0].color = glm::vec3(0.35f, 0.80f, 1.00f);
    esferas[0].duracionTramo = 3.5f;
    esferas[0].rng.seed(1234);
    esferas[1].color = glm::vec3(0.58f, 0.25f, 0.95f);
    esferas[1].duracionTramo = 4.5f;
    esferas[1].rng.seed(5678);
    for (auto& e : esferas) e.iniciar(regionMin, regionMax);
}

void SistemaEsferas::actualizar(float dt) {
    for (auto& e : esferas) e.actualizar(dt, regionMin, regionMax);
}

void SistemaEsferas::vincularLuces(GLuint programaModelo) {
    locLuzPos = glGetUniformLocation(programaModelo, "uLuzEsferaPos");
    locLuzColor = glGetUniformLocation(programaModelo, "uLuzEsferaColor");
    locLuzRadio = glGetUniformLocation(programaModelo, "uLuzEsferaRadio");
    locLuzIntensidad = glGetUniformLocation(programaModelo, "uLuzEsferaIntensidad");
}

void SistemaEsferas::enviarLuces() {
    float pos[6], col[6];
    for (int i = 0; i < 2; i++) {
        for (int eje = 0; eje < 3; eje++) {
            pos[i * 3 + eje] = esferas[i].posicion[eje];
            col[i * 3 + eje] = esferas[i].color[eje];
        }
    }
    glUniform3fv(locLuzPos, 2, pos);
    glUniform3fv(locLuzColor, 2, col);
    glUniform1f(locLuzRadio, radioLuz);
    glUniform1f(locLuzIntensidad, intensidadLuz);
}

void SistemaEsferas::dibujar(const glm::mat4& projection, const glm::mat4& view) {
    if (!programa || !vao) return;
    glBindVertexArray(vao);
    glUseProgram(programa);
    for (auto& e : esferas) {
        glm::mat4 modelo = glm::translate(glm::mat4(1.0f), e.posicion);
        modelo = glm::scale(modelo, glm::vec3(radio));
        glm::mat4 mvp = projection * view * modelo;
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3fv(locColor, 1, glm::value_ptr(e.color));
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void SistemaEsferas::crearMalla(int stacks, int sectors) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    const float PI = 3.14159265358979f;
    for (int i = 0; i <= stacks; i++) {
        float phi = PI * i / stacks;
        for (int j = 0; j <= sectors; j++) {
            float theta = 2.0f * PI * j / sectors;
            v.push_back(sin(phi) * cos(theta));
            v.push_back(cos(phi));
            v.push_back(sin(phi) * sin(theta));
        }
    }
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < sectors; j++) {
            unsigned int a = i * (sectors + 1) + j;
            unsigned int b = a + sectors + 1;
            idx.insert(idx.end(), { a, b, a + 1, a + 1, b, b + 1 });
        }
    }
    indexCount = (int)idx.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void SistemaEsferas::liberar() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteProgram(programa);
    vao = vbo = ibo = programa = 0;
}

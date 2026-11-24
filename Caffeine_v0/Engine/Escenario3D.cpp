#include "Escenario3D.h"
#include "SistemaEsferas.h"
#include "ShaderUtils.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

bool Escenario3D::init(const std::string& rutaGLTF) {
    programa_ = crearProgramaDesdeArchivos("Shaders/Modelo.vert", "Shaders/Modelo.frag");
    if (!programa_) return false;
    escena_ = cargarGLTF(rutaGLTF);
    if (escena_.meshes.empty()) {
        std::cerr << "Escenario3D: modelo sin mallas: " << rutaGLTF << '\n';
        return false;
    }
    locMVP_ = glGetUniformLocation(programa_, "uMVP");
    locModel_ = glGetUniformLocation(programa_, "uModel");
    locHasTex_ = glGetUniformLocation(programa_, "uHasTex");
    locTex_ = glGetUniformLocation(programa_, "uTex");
    locColor_ = glGetUniformLocation(programa_, "uColor");
    locLightPos_ = glGetUniformLocation(programa_, "uLightPos");
    locLightRadius_ = glGetUniformLocation(programa_, "uLightRadius");
    const glm::vec3 centro = (escena_.bmin + escena_.bmax) * 0.5f;
    const glm::vec3 tam = escena_.bmax - escena_.bmin;
    lightPos_ = glm::vec3(centro.x, escena_.bmin.y + tam.y * 0.8f, centro.z);
    lightRadius_ = glm::length(tam) * 0.75f;
    return true;
}

void Escenario3D::dibujar(const glm::mat4& projection, const glm::mat4& view,
                          SistemaEsferas& luces) {
    if (!programa_) return;
    glUseProgram(programa_);
    // Iluminacion de las esferas requiere el shader de modelos activo.
    luces.enviarLuces();
    glUniform3fv(locLightPos_, 1, glm::value_ptr(lightPos_));
    glUniform1f(locLightRadius_, lightRadius_);
    for (const auto& mesh : escena_.meshes) {
        const glm::mat4& model = mesh.modelMatrix;
        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(locMVP_, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(locModel_, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(locColor_, 1, glm::value_ptr(mesh.color));
        if (mesh.texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.texture);
            glUniform1i(locTex_, 0);
            glUniform1i(locHasTex_, 1);
        } else {
            glUniform1i(locHasTex_, 0);
        }
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void Escenario3D::liberar() {
    liberarGLTF(escena_);
    if (programa_) glDeleteProgram(programa_);
    programa_ = 0;
}

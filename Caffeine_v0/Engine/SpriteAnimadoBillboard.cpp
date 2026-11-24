#include "SpriteAnimadoBillboard.h"
#include "ShaderUtils.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>

void SpriteAnimadoBillboard::init(const std::string& carpeta, const std::string& prefijo) {
    programa = crearProgramaDesdeArchivos("Shaders/Billboard.vert", "Shaders/Billboard.frag");
    if (!programa) return;
    glGenVertexArrays(1, &vao); // El shader genera los 6 vertices con gl_VertexID.

    for (int i = 0; i < NUM_FRAMES; ++i) {
        std::string ruta = carpeta + prefijo + std::to_string(i + 1) + ".png";
        int ancho = 0, alto = 0;
        fotogramas[i] = cargarPNG(ruta, ancho, alto);
        if (fotogramas[i] && (ancho != int(anchoOriginal) || alto != int(altoOriginal)))
            std::cout << "Dimensiones inesperadas en " << ruta << '\n';
    }
}

void SpriteAnimadoBillboard::dibujar(float tiempo, const glm::mat4& projection,
                                    const glm::mat4& view) {
    if (!visible || !programa || !vao || escala <= 0.0f || fps <= 0.0f) return;

    // El tiempo selecciona un PNG entre los 6, en bucle.
    int frameActual = static_cast<int>(std::max(0.0f, tiempo) * fps) % NUM_FRAMES;
    GLuint texturaActual = fotogramas[frameActual];
    if (!texturaActual) return;

    glUseProgram(programa);
    glBindVertexArray(vao);
    glUniformMatrix4fv(glGetUniformLocation(programa, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(programa, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(glGetUniformLocation(programa, "uPosicion"), x, y, z);
    // 672 pixeles equivalen a escala unidades del mundo. Mantener la proporcion original.
    glUniform1f(glGetUniformLocation(programa, "uAlto"), escala * (altoOriginal / 672.0f));
    glUniform1f(glGetUniformLocation(programa, "uAspecto"), anchoOriginal / altoOriginal);
    glUniform1i(glGetUniformLocation(programa, "uFotograma"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaActual);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SpriteAnimadoBillboard::liberar() {
    glDeleteTextures(NUM_FRAMES, fotogramas);
    for (GLuint& textura : fotogramas) textura = 0;
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programa);
    vao = programa = 0;
}

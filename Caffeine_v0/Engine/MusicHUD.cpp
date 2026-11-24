#include "MusicHUD.h"
#include "ShaderUtils.h"

void MusicHUD::init(const std::string& rutaPNG) {
    programa = crearProgramaDesdeArchivos("Shaders/Onda.vert", "Shaders/Onda.frag");
    if (!programa) return;
    glGenVertexArrays(1, &vao); // Las ondas y la caratula usan gl_VertexID.
    programaImagen = crearProgramaDesdeArchivos("Shaders/Caratula.vert", "Shaders/Caratula.frag");
    texturaImagen = cargarPNG(rutaPNG, imagenW, imagenH);
}

void MusicHUD::dibujar(float tiempo, int ancho, int alto) {
    if (ancho <= 0 || alto <= 0 || !vao) return;

    glUseProgram(programa);
    glBindVertexArray(vao);

    // Enviamos al vertex shader los parametros de la animacion.
    // No se recalculan vertices en CPU ni se actualiza ningun VBO.
    glUniform2f(glGetUniformLocation(programa, "uResolucion"), (float)ancho, (float)alto);
    glUniform1f(glGetUniformLocation(programa, "uTiempo"), tiempo);
    glUniform1f(glGetUniformLocation(programa, "uFraccionHUD"), fraccionHUD);
    glUniform1f(glGetUniformLocation(programa, "uBPM"), bpm);
    glUniform1f(glGetUniformLocation(programa, "uVelocidad"), velocidad);
    glUniform1f(glGetUniformLocation(programa, "uAmplitud"), amplitudBase);
    glUniform1f(glGetUniformLocation(programa, "uTurbulencia"), turbulencia);
    glUniform1f(glGetUniformLocation(programa, "uFuerzaBeat"), fuerzaBeat);
    glUniform1f(glGetUniformLocation(programa, "uDesfaseBeat"), desfaseBeat);

    // La onda secundaria es opcional. uCapa = 1 selecciona su
    // desfase, escala y grosor; el alpha 0.85 deja ver el escenario.
    // Se dibuja primero para que la azul quede visualmente encima.
    if (dobleOnda) {
        glUniform1f(glGetUniformLocation(programa, "uCapa"), 1.0f);
        glUniform4f(glGetUniformLocation(programa, "uColorOnda"),
            0.73f, 0.30f, 0.76f, 0.85f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (512 + 1) * 2);
    }
    // uCapa = 0 selecciona la onda principal, con amplitud completa.
    // Cada llamada emite 1026 vertices = (512 segmentos + 1) * 2.
    // GL_TRIANGLE_STRIP une los pares formando una cinta continua.
    glUniform1f(glGetUniformLocation(programa, "uCapa"), 0.0f);
    glUniform4f(glGetUniformLocation(programa, "uColorOnda"),
        0.25f, 0.34f, 1.00f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (512 + 1) * 2);

    // DIBUJAR PNG DESPUES DE AMBAS ONDAS: asi la caratula queda encima.
    // Las partes transparentes del PNG permiten ver las ondas de detras.
    dibujarImagen(ancho, alto);
}

void MusicHUD::dibujarImagen(int ancho, int alto) {
    if (!mostrarImagen || !programaImagen || !texturaImagen ||
        imagenW <= 0 || imagenH <= 0) return;

    // Anchura relativa al framebuffer; altura segun el aspecto REAL.
    // Con imagen 1213x620 y ventana 1280x720: ~370x189 px.
    float anchoPx = (float)ancho * imagenAnchoRel;
    float altoPx = anchoPx * (float)imagenH / (float)imagenW;
    float xPx = (float)ancho * imagenXRel;
    float yPx = (float)alto * imagenMargenInferiorRel;

    glUseProgram(programaImagen);
    glUniform2f(glGetUniformLocation(programaImagen, "uResolucion"),
        (float)ancho, (float)alto);
    glUniform2f(glGetUniformLocation(programaImagen, "uPosicionPx"), xPx, yPx);
    glUniform2f(glGetUniformLocation(programaImagen, "uTamanoPx"), anchoPx, altoPx);
    glUniform1i(glGetUniformLocation(programaImagen, "uImagen"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaImagen);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Dos triangulos, sin VBO.
}

void MusicHUD::liberar() {
    if (texturaImagen) glDeleteTextures(1, &texturaImagen);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (programaImagen) glDeleteProgram(programaImagen);
    if (programa) glDeleteProgram(programa);
    texturaImagen = 0;
    vao = 0;
    programaImagen = 0;
    programa = 0;
}

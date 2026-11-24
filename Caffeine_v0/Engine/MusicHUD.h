#pragma once
#include <glad/glad.h>
#include <string>

// Overlay 2D sin fondo: doble onda senoidal y caratula RGBA.
class MusicHUD {
public:
    // PARAMETROS EDITABLES DEL VISUALIZADOR
    float bpm = 115.0f;
    float velocidad = 22.0f;
    float amplitudBase = 0.59f;
    float turbulencia = 1.00f;
    float fuerzaBeat = 0.89f;
    bool dobleOnda = true;
    float desfaseBeat = 0.0f;
    float fraccionHUD = 0.25f;

    // POSICION EDITABLE DE LA IMAGEN (relativa al framebuffer)
    float imagenXRel = 68.0f / 1280.0f;
    float imagenMargenInferiorRel = 35.0f / 720.0f;
    float imagenAnchoRel = 370.0f / 1280.0f;
    bool mostrarImagen = true;

    MusicHUD() = default;
    MusicHUD(const MusicHUD&) = delete;
    MusicHUD& operator=(const MusicHUD&) = delete;
    void init(const std::string& rutaPNG = "image song.png");
    // main desactiva profundidad y mantiene la mezcla alfa antes de dibujar.
    void dibujar(float tiempo, int ancho, int alto);
    void liberar();
private:
    GLuint programa = 0, vao = 0;
    GLuint programaImagen = 0, texturaImagen = 0;
    int imagenW = 0, imagenH = 0;
    void dibujarImagen(int ancho, int alto);
};

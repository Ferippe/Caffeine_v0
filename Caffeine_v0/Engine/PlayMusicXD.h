#pragma once
#include <miniaudio.h>
#include <string>

// Reproduccion de MP3 en streaming y bucle usando miniaudio.c.
class PlayMusicXD {
public:
    std::string archivo = "song.mp3";
    float volumen = 0.80f;
    bool repetir = true;

    PlayMusicXD() = default;
    PlayMusicXD(const PlayMusicXD&) = delete;
    PlayMusicXD& operator=(const PlayMusicXD&) = delete;
    bool init();
    void liberar();
private:
    ma_engine motor{};
    ma_sound sonido{};
    bool motorInicializado = false;
    bool sonidoInicializado = false;
};

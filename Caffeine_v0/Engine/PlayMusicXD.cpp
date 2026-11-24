#include "PlayMusicXD.h"
#include <iostream>

bool PlayMusicXD::init() {
    if (motorInicializado) return sonidoInicializado;

    // 1) Abrimos el motor de audio predeterminado del sistema.
    ma_result resultado = ma_engine_init(nullptr, &motor);
    if (resultado != MA_SUCCESS) {
        std::cout << "playMusicXD: no se pudo iniciar el motor de audio ("
            << resultado << ").\n";
        return false;
    }
    motorInicializado = true;

    // 2) Abrimos el MP3. STREAM evita almacenar toda la cancion en RAM.
    // NO_SPATIALIZATION hace que el sonido se escuche como musica de fondo
    // y no cambie de volumen/posicion al mover la camara por el escenario.
    resultado = ma_sound_init_from_file(
        &motor,
        archivo.c_str(),
        MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION,
        nullptr,
        nullptr,
        &sonido
    );
    if (resultado != MA_SUCCESS) {
        std::cout << "playMusicXD: no se pudo abrir '" << archivo
            << "' (" << resultado << "). Revisa la ruta de trabajo.\n";
        ma_engine_uninit(&motor);
        motorInicializado = false;
        return false;
    }
    sonidoInicializado = true;

    // 3) Configuramos repeticion y volumen ANTES de iniciar el sonido.
    ma_sound_set_looping(&sonido, repetir ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(&sonido, volumen);

    // 4) Comenzamos la musica. No bloquea el bucle de OpenGL.
    resultado = ma_sound_start(&sonido);
    if (resultado != MA_SUCCESS) {
        std::cout << "playMusicXD: fallo al reproducir '" << archivo
            << "' (" << resultado << ").\n";
        liberar();
        return false;
    }

    std::cout << "playMusicXD: reproduciendo '" << archivo
        << "' en bucle.\n";
    return true;
}

void PlayMusicXD::liberar() {
    if (sonidoInicializado) {
        ma_sound_uninit(&sonido);
        sonidoInicializado = false;
    }
    if (motorInicializado) {
        ma_engine_uninit(&motor);
        motorInicializado = false;
    }
}

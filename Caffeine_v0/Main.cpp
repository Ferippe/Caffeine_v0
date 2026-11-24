#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/Logo.h"
#include "Engine/Escenario3D.h"
#include "Engine/CamaraFPS.h"
#include "Engine/SistemaEsferas.h"
#include "Engine/SpriteAnimadoBillboard.h"
#include "Engine/MusicHUD.h"
#include "Engine/PlayMusicXD.h"

// Caffeine: punto de entrada, configuracion y coordinacion de modulos.
// La carga GLTF, los shaders y el render de cada sistema estan en Engine/.
constexpr int ANCHO_VENTANA = 1280;
constexpr int ALTO_VENTANA = 720;

int main() {
    mostrarLogo();
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(ANCHO_VENTANA, ALTO_VENTANA,
        "Caffeine_v0 - Concierto En Vivo :P", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE); // Todas las capas son de doble cara.

    // ======================= ESCENARIO =======================
    Escenario3D escenario;
    if (!escenario.init("every_gig_ever/scene.gltf")) {
        escenario.liberar();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    const RenderModel& modelo = escenario.modelo();
    const glm::vec3 centro = (modelo.bmin + modelo.bmax) * 0.5f;

    // ======================== CAMARA =========================
    CamaraFPS camara;
    camara.init(modelo, ANCHO_VENTANA, ALTO_VENTANA);

    // ===================== LUCES (ESFERITAS QUE SIGUEN UN BEZIER) ==========================
    SistemaEsferas esferas;
    esferas.init(modelo.bmin, modelo.bmax);
    esferas.vincularLuces(escenario.programa());

    // ================ PERSONAJE 1 (EDITABLE) ======================
    SpriteAnimadoBillboard spriteEscenario;
    spriteEscenario.x = centro.x;
    spriteEscenario.y = modelo.bmin.y + 0.5f; // Altura de los pies
    spriteEscenario.z = centro.z;
    spriteEscenario.init("", "anim");

    // ================ PERSONAJE 2 (EDITABLE) ======================
    SpriteAnimadoBillboard spriteEscenario2;
    spriteEscenario2.x = spriteEscenario.x;
    spriteEscenario2.y = spriteEscenario.y;
    spriteEscenario2.z = spriteEscenario.z + 3.6f; // Frente del 1 (+Z)
    spriteEscenario2.anchoOriginal = 920.0f;
    spriteEscenario2.altoOriginal = 849.0f;
    spriteEscenario2.init("", "anim_B");

    // ======================== MUSIC HUD ==========================
    MusicHUD musicHUD;
    musicHUD.init("image song.png");

    // ======================== AUDIO MP3 ==========================
    PlayMusicXD playMusicXD;
    playMusicXD.init(); // Si falla el audio, la escena sigue funcionando.

    float lastTime = float(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        float now = float(glfwGetTime());
        float dt = now - lastTime;
        lastTime = now;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        int fbAncho=0, fbAlto=0;
        glfwGetFramebufferSize(window, &fbAncho, &fbAlto);
        if (fbAncho <= 0 || fbAlto <= 0) { glfwPollEvents(); continue; }
        glViewport(0, 0, fbAncho, fbAlto);
        // 1. Mundo opaco: profundidad activa, sin transparencia.
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camara.actualizar(window, dt);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f),
            float(fbAncho)/float(fbAlto), 0.1f, 100.0f);
        glm::mat4 view = camara.vista();

        esferas.actualizar(dt);
        escenario.dibujar(projection, view, esferas);
        esferas.dibujar(projection, view);
        // 2. Sprites transparentes: el escenario puede ocultarlos, pero no escriben profundidad.
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        spriteEscenario.dibujar(now, projection, view);
        spriteEscenario2.dibujar(now, projection, view);
        // 3. HUD: encima de todo, con la misma mezcla alpha.
        glDisable(GL_DEPTH_TEST);
        musicHUD.dibujar(now, fbAncho, fbAlto);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar con OpenGL aun activo. El audio no depende de OpenGL.
    glUseProgram(0);
    glBindVertexArray(0);
    playMusicXD.liberar();
    spriteEscenario.liberar();
    spriteEscenario2.liberar();
    musicHUD.liberar();
    esferas.liberar();
    escenario.liberar();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

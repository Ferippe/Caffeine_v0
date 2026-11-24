#pragma once
#include "GLTFModel.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

class SistemaEsferas;

// Contiene el shader de modelos, las mallas GLTF y la luz principal.
class Escenario3D {
public:
    bool init(const std::string& rutaGLTF);
    void dibujar(const glm::mat4& projection, const glm::mat4& view,
                 SistemaEsferas& luces);
    void liberar();

    const RenderModel& modelo() const { return escena_; }
    GLuint programa() const { return programa_; }
private:
    RenderModel escena_;
    GLuint programa_ = 0;
    GLint locMVP_=-1, locModel_=-1, locHasTex_=-1, locTex_=-1;
    GLint locColor_=-1, locLightPos_=-1, locLightRadius_=-1;
    glm::vec3 lightPos_{0.0f};
    float lightRadius_=1.0f;
};

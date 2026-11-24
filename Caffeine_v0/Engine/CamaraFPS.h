#pragma once
#include <glm/glm.hpp>
struct GLFWwindow;
struct RenderModel;

// Conserva la posicion y orientacion de la camara original de Caffeine.
class CamaraFPS {
public:
    void init(const RenderModel& modelo, int ancho, int alto);
    void actualizar(GLFWwindow* ventana, float dt);
    glm::mat4 vista() const;
    glm::vec3 posicion() const { return camPos_; }
private:
    glm::vec3 camPos_{0.0f};
    glm::vec3 camFront_{0.0f,0.0f,-1.0f};
    glm::vec3 camUp_{0.0f,1.0f,0.0f};
    float camSpeed_ = 2.5f;
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;
    float mouseSens_ = 0.1f;
    double lastX_ = 0.0, lastY_ = 0.0;
    bool firstMouse_ = true;
};

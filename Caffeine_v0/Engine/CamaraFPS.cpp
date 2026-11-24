#include "CamaraFPS.h"
#include "GLTFModel.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

void CamaraFPS::init(const RenderModel& modelo, int ancho, int alto) {
    const glm::vec3 centro = (modelo.bmin + modelo.bmax) * 0.5f;
    // Identico al main original: frente en +Z, a 1.7 unidades de altura.
    const glm::vec3 frente(centro.x, modelo.bmin.y, modelo.bmax.z);
    camPos_ = glm::vec3(frente.x, frente.y + 1.7f, frente.z + 2.0f);
    lastX_ = ancho / 2.0;
    lastY_ = alto / 2.0;
    firstMouse_ = true;
}

void CamaraFPS::actualizar(GLFWwindow* ventana, float dt) {
    double mouseX=0.0, mouseY=0.0;
    glfwGetCursorPos(ventana, &mouseX, &mouseY);
    if (firstMouse_) {
        lastX_ = mouseX;
        lastY_ = mouseY;
        firstMouse_ = false;
    }
    yaw_ += float(mouseX - lastX_) * mouseSens_;
    pitch_ += float(lastY_ - mouseY) * mouseSens_;
    lastX_ = mouseX;
    lastY_ = mouseY;
    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
    camFront_ = glm::normalize(glm::vec3(
        std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_)),
        std::sin(glm::radians(pitch_)),
        std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_))));
    float v = camSpeed_ * dt;
    glm::vec3 right = glm::normalize(glm::cross(camFront_,camUp_));
    if (glfwGetKey(ventana, GLFW_KEY_W) == GLFW_PRESS) camPos_ += camFront_ * v;
    if (glfwGetKey(ventana, GLFW_KEY_S) == GLFW_PRESS) camPos_ -= camFront_ * v;
    if (glfwGetKey(ventana, GLFW_KEY_A) == GLFW_PRESS) camPos_ -= right * v;
    if (glfwGetKey(ventana, GLFW_KEY_D) == GLFW_PRESS) camPos_ += right * v;
}

glm::mat4 CamaraFPS::vista() const {
    return glm::lookAt(camPos_, camPos_+camFront_, camUp_);
}

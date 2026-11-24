#version 330 core
in vec2 vUV;
uniform sampler2D uFotograma;
out vec4 FragColor;
void main() {
    vec4 color = texture(uFotograma, vUV);
    if (color.a < 0.01) discard;
    FragColor = color;
}

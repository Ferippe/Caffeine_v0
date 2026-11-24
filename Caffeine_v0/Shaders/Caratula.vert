#version 330 core
uniform vec2 uResolucion;
uniform vec2 uPosicionPx;
uniform vec2 uTamanoPx;
out vec2 vUV;
void main() {
    const vec2 esquinas[6] = vec2[6](
        vec2(0,0), vec2(1,0), vec2(1,1),
        vec2(0,0), vec2(1,1), vec2(0,1)
    );
    vec2 esquina = esquinas[gl_VertexID];
    vec2 pixeles = uPosicionPx + esquina * uTamanoPx;
    gl_Position = vec4(pixeles / uResolucion * 2.0 - 1.0, 0.0, 1.0);
    // stbi_load entrega primero la fila SUPERIOR del archivo. Como OpenGL
    // interpreta esa fila como V=0, invertimos V en vez de modificar el PNG.
    vUV = vec2(esquina.x, 1.0 - esquina.y);
}

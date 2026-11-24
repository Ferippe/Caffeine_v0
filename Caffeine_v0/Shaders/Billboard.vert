#version 330 core
uniform mat4 uProjection;
uniform mat4 uView;
uniform vec3 uPosicion;
uniform float uAlto;
uniform float uAspecto; // Ancho/alto ORIGINAL del personaje que se dibuja.
out vec2 vUV;
void main() {
    const vec2 esquinas[6] = vec2[6](
        vec2(-0.5, 0.0), vec2( 0.5, 0.0), vec2( 0.5, 1.0),
        vec2(-0.5, 0.0), vec2( 0.5, 1.0), vec2(-0.5, 1.0)
    );
    vec2 esquina = esquinas[gl_VertexID];
    float ancho = uAlto * uAspecto;

    // Punto de los pies en espacio de camara; no se mueve al girar la vista.
    vec4 centroCamara = uView * vec4(uPosicion, 1.0);
    vec4 verticeCamara = centroCamara +
                         vec4(esquina.x * ancho, esquina.y * uAlto, 0.0, 0.0);
    gl_Position = uProjection * verticeCamara;

    // stb_image entrega la primera fila ARRIBA y OpenGL la interpreta abajo.
    // Invertimos V para ver correctamente el PNG, sin voltearlo al cargar.
    vUV = vec2(esquina.x + 0.5, 1.0 - esquina.y);
}

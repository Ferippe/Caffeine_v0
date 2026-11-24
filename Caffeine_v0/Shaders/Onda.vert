#version 330 core
// Variables enviadas desde C++ cada frame (uniforms).
// uResolucion: ancho y alto reales del framebuffer, en pixeles.
// uTiempo: segundos transcurridos desde el inicio del motor.
// uBPM: golpes por minuto (la duracion de un beat es 60 / BPM).
// uVelocidad: rapidez con la que corren las ondulaciones lateralmente.
// uAmplitud: altura base de las ondas; uTurbulencia: detalle pequeno.
// uFuerzaBeat: cuanto se expande con cada beat simulado.
// uDesfaseBeat: retraso en segundos para alinear un beat con la musica.
// uCapa: 0 = onda azul principal; 1 = secundaria morada.
// uFraccionHUD: altura relativa de la zona inferior que contiene el centro.
uniform vec2 uResolucion;
uniform float uTiempo;
uniform float uBPM;
uniform float uVelocidad;
uniform float uAmplitud;
uniform float uTurbulencia;
uniform float uFuerzaBeat;
uniform float uDesfaseBeat;
uniform float uCapa;
uniform float uFraccionHUD;

const float PI = 3.141592653589793;
// Se divide el ancho de pantalla en 512 tramos. El draw usa 513 muestras,
// porque incluye los dos extremos (u = 0 y u = 1).
const float MUESTRAS = 512.0;

// Devuelve el DESPLAZAMIENTO VERTICAL (pixeles) de la onda en u.
// u va de 0 (borde izquierdo) a 1 (borde derecho).
// Las tres senoides se suman: ola grande + detalle + microdetalle.
float alturaOnda(float u) {
    // Convertimos segundos a fase musical: 2*pi radianes por beat.
    // Restar uDesfaseBeat desplaza el momento del golpe.
    float fase = 2.0 * PI * (uTiempo - uDesfaseBeat) * uBPM / 60.0;
    // cos() da un maximo en cada beat. Elevar a 5 concentra el pico,
    // para que la expansion sea breve en lugar de una oscilacion uniforme.
    float pulso = pow(0.5 + 0.5 * cos(fase), 5.0);

    // Altura en pixeles: 15 es el minimo constante; el resto depende
    // de la amplitud elegida y del pulso. Se escala respecto a 720p
    // para mantener una proporcion similar en otras resoluciones.
    // No representa el volumen real de ninguna cancion.
    float amplitudPx = (15.0 + uAmplitud * 51.0 + uFuerzaBeat * 35.0 * pulso)
                       * uResolucion.y / 720.0;
    // Diferente fase para que las dos capas no sigan exactamente
    // el mismo trazo. Capa 1 = morada; capa 0 = azul.
    float desfase = uCapa > 0.5 ? 1.7 : 0.0;
    float t = uTiempo;
    // La frecuencia espacial (32) decide cuantas crestas hay a lo ancho.
    // Restar tiempo * velocidad hace que la onda avance horizontalmente.
    float ondaPrincipal = sin(u * 32.0 - t * uVelocidad + desfase);
    // Segunda senoide: ondulaciones mas pequenas y rapidas.
    // Turbulencia = 0 elimina por completo estas variaciones.
    float detalle = uTurbulencia * 0.55 *
                    sin(u * 71.0 - t * uVelocidad * 1.65 + desfase * 1.3);
    // Tercera senoide: vibraciones finas en sentido contrario.
    // Da un movimiento menos perfecto y mas parecido a agua.
    float microDetalle = uTurbulencia * 0.18 *
                         sin(u * 117.0 + t * uVelocidad * 1.95 + desfase);
    // Multiplicadores espaciales: cambian ligeramente la altura
    // segun la posicion horizontal para evitar crestas identicas.
    float envolvente = 0.83 + 0.17 * sin(PI * u);
    float variacion = 0.80 + 0.20 * cos(u * 18.0 - t * 0.8);
    // La onda morada usa el 53% de la altura de la azul.
    float escala = uCapa > 0.5 ? 0.53 : 1.0;
    // Resultado final: forma combinada * amplitud * variaciones * capa.
    return (ondaPrincipal + detalle + microDetalle) *
           amplitudPx * envolvente * variacion * escala;
}

void main() {
    // Se emiten 2 vertices por cada x: uno a cada lado del grosor.
    // gl_VertexID: 0,1 pertenecen a la muestra 0; 2,3 a la muestra 1...
    int punto = gl_VertexID / 2;
    // Alternar -1/+1 coloca los bordes de la cinta alrededor de la curva.
    float lado = (gl_VertexID % 2 == 0) ? -1.0 : 1.0;
    // Convertimos el indice a posicion horizontal normalizada [0,1].
    float u = float(punto) / MUESTRAS;
    float x = u * uResolucion.x;
    // Centro vertical: mitad de la zona inferior del HUD. OpenGL mide
    // y desde abajo en estas coordenadas; alturaOnda suma las crestas.
    // No existe ningun rectangulo de fondo: solo se dibujan las lineas.
    float y = uResolucion.y * uFraccionHUD * 0.5 + alturaOnda(u);

    // Aproximamos la pendiente de la curva alrededor de u (diferencia
    // centrada) para calcular la direccion PERPENDICULAR a la linea.
    // Desplazar vertices sobre esta normal crea una linea gruesa
    // incluso en OpenGL Core, donde glLineWidth no es muy fiable.
    float epsilon = 1.0 / uResolucion.x;
    float pendiente = (alturaOnda(u + epsilon) - alturaOnda(u - epsilon)) * 0.5;
    vec2 normal = normalize(vec2(-pendiente, 1.0));
    // Grosor total en pixeles: morada 2.5, azul 3.3.
    float grosor = (uCapa > 0.5) ? 2.5 : 3.3;
    vec2 posPx = vec2(x, y) + normal * (lado * grosor * 0.5);
    // Pasamos de pixeles a coordenadas NDC de OpenGL [-1,1].
    // Como es HUD, no usamos matrices de vista ni perspectiva.
    vec2 clip = posPx / uResolucion * 2.0 - 1.0;
    gl_Position = vec4(clip, 0.0, 1.0);
}

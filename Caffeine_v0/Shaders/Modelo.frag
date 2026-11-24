#version 330 core
in vec2 vUV;
in vec3 vNormal;
in vec3 vWorldPos;
out vec4 FragColor;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec4 uColor;
uniform vec3 uLightPos;
uniform float uLightRadius;
// --- Luces de las esferas (el codigo C++ de estas luces esta en la seccion ESFERAS) ---
#define NUM_LUCES_ESFERA 2
uniform vec3 uLuzEsferaPos[NUM_LUCES_ESFERA];
uniform vec3 uLuzEsferaColor[NUM_LUCES_ESFERA];
uniform float uLuzEsferaRadio;
uniform float uLuzEsferaIntensidad;
void main()
{
   vec3 norm = normalize(vNormal);
   if (!gl_FrontFacing) norm = -norm;
   vec3 toLight = uLightPos - vWorldPos;
   float dist = length(toLight);
   vec3 lightDir = toLight / dist;
   float att = 1.0 / (1.0 + 1.5 * pow(dist / uLightRadius, 2.0));
   vec3 luz = vec3(0.30 + 1.3 * max(dot(norm, lightDir), 0.0) * att);
   // Cada esfera aporta luz de su color; las contribuciones se SUMAN (mezcla aditiva)
   for (int i = 0; i < NUM_LUCES_ESFERA; i++) {
       vec3 d = uLuzEsferaPos[i] - vWorldPos;
       float dist2 = length(d);
       float f = clamp(1.0 - dist2 / uLuzEsferaRadio, 0.0, 1.0);
       luz += uLuzEsferaColor[i] * uLuzEsferaIntensidad * f * f * max(dot(norm, d / max(dist2, 0.0001)), 0.0);
   }
   
   vec4 texColor = uColor;
   if (uHasTex == 1) {
       texColor *= texture(uTex, vUV);
   }
   if (texColor.a < 0.1) discard;
   FragColor = vec4(texColor.rgb * luz, texColor.a);
}

# Caffeine_v0

**Desarrollado por:** William Felipe Varela Rodriguez  
**Código:** 6014422  

---

## Descripción

**Caffeine_v0** utiliza una arquitectura modular, donde cada 
componente del motor cumple una función específica. El archivo 
`main.cpp` se encarga de inicializar y coordinar todos los módulos.

---
## Demo del motor

Demostración de **Caffeine_v0** en ejecución, mostrando la 
integración del escenario 3D, iluminación dinámica, sprites 
animados, cámara FPS y visualización musical en tiempo real.

[![Caffeine_v0 - Tech Demo](https://img.youtube.com/vi/aqV4x1tTGus/maxresdefault.jpg)](https://youtu.be/aqV4x1tTGus?si=2QFevc69TpP48MKw)

**[Ver demostración completa en YouTube](https://youtu.be/aqV4x1tTGus?si=2QFevc69TpP48MKw)**

---
## Módulos del motor

| Módulo | Descripción |
| :--- | :--- |
| `main.cpp` | Inicialización del motor y bucle principal de renderizado. |
| `CamaraFPS` | Movimiento y orientación de la cámara mediante teclado y ratón. |
| `Escenario3D` | Renderizado e iluminación del escenario 3D. |
| `GLTFModel` | Carga de modelos GLTF/GLB, geometría, normales y texturas. |
| `SistemaEsferas` | Generación de esferas y luces móviles mediante curvas Bézier. |
| `SpriteAnimadoBillboard` | Animación de sprites 2D orientados hacia la cámara. |
| `MusicHUD` | Renderizado de las ondas musicales y la carátula de la canción. |
| `PlayMusicXD` | Reproducción de audio MP3 mediante miniaudio. |
| `ShaderUtils` | Carga y compilación de shaders y carga de texturas. |
| `Logo` | Visualización del logo del proyecto en la consola. |

---



## Shaders

Los shaders están escritos en **GLSL** y se almacenan 
de forma independiente del código C++.

| Shader | Función |
| :--- | :--- |
| `Modelo` | Renderizado, texturizado e iluminación del modelo 3D. |
| `Esferas` | Renderizado de las esferas de luz. |
| `Billboard` | Renderizado de sprites orientados hacia la cámara. |
| `Onda` | Generación y animación de las ondas musicales. |
| `Caratula` | Renderizado de la portada de la canción. |

### Estructura de los shaders

Cada shader está compuesto por dos archivos:

- **Vertex Shader (`.vert`):** Procesamiento de vértices y 
  transformaciones geométricas.
- **Fragment Shader (`.frag`):** Procesamiento de fragmentos, 
  colores, texturas e iluminación.

---

## Recursos multimedia

> [!IMPORTANT]
> Por motivos de derechos de autor, los sprites, la carátula
> y la música utilizados en la demostración original no se 
> distribuyen en este repositorio.
>
> En su lugar, se incluyen recursos multimedia de sustitución
> (*placeholders*) para permitir la ejecución del motor.

¡Siéntete libre de añadir tus propios sprites y música para 
personalizar la escena!

### Sprites animados

Cada personaje utiliza una secuencia de seis fotogramas en 
formato PNG. Para reemplazarlos, conserva los nombres, las 
resoluciones y las ubicaciones de los archivos originales.

#### Personaje 1

**Resolución requerida:** 538 × 672 píxeles.

| Fotograma | Nombre del archivo |
| :---: | :--- |
| 1 | `anim1.png` |
| 2 | `anim2.png` |
| 3 | `anim3.png` |
| 4 | `anim4.png` |
| 5 | `anim5.png` |
| 6 | `anim6.png` |

#### Personaje 2

**Resolución requerida:** 920 × 849 píxeles.

| Fotograma | Nombre del archivo |
| :---: | :--- |
| 1 | `anim_B1.png` |
| 2 | `anim_B2.png` |
| 3 | `anim_B3.png` |
| 4 | `anim_B4.png` |
| 5 | `anim_B5.png` |
| 6 | `anim_B6.png` |

> [!TIP]
> Se recomienda utilizar imágenes PNG con fondo transparente
> para conservar la correcta integración de los personajes 
> dentro del escenario 3D.

### Música y sincronización

El motor permite reproducir archivos de audio en formato MP3
mediante la biblioteca **miniaudio**.

Para utilizar una canción personalizada:

1. Selecciona un archivo de audio en formato MP3.
2. Renombra el archivo como `song.mp3`.
3. Reemplaza el archivo de audio incluido en el repositorio.
4. Abre el archivo `MusicHUD.h`.
5. Ajusta la variable `bpm` al tempo de la canción seleccionada
   para sincronizar la animación de las ondas musicales.

> [!NOTE]
> El audio de sustitución incluido en el repositorio tiene
> un tempo de **115 BPM**.

---

## Créditos y recursos de terceros

### Modelo 3D del escenario

| Información | Detalle |
| :--- | :--- |
| **Título** | Every gig, ever... |
| **Autor** | [PippyJ](https://sketchfab.com/PippyJ) |
| **Plataforma** | Sketchfab |
| **Licencia** | [Creative Commons Attribution 4.0 International](https://creativecommons.org/licenses/by/4.0/) |
| **Uso comercial** | Permitido bajo las condiciones de la licencia. |
| **Atribución** | Obligatoria. |

[Ver modelo original en Sketchfab](https://sketchfab.com/3d-models/every-gig-ever-c8cddb7d1538477380445d2f0d2531b2)

#### Atribución original

> This work is based on "Every gig, ever..." 
> (https://sketchfab.com/3d-models/every-gig-ever-c8cddb7d1538477380445d2f0d2531b2) 
> by PippyJ (https://sketchfab.com/PippyJ) licensed under CC-BY-4.0 
> (http://creativecommons.org/licenses/by/4.0/)

El modelo 3D pertenece a su respectivo autor y se distribuye 
bajo las condiciones de su licencia original. Cualquier 
redistribución deberá conservar la atribución correspondiente.

---

**Caffeine_v0** — Developed by William Felipe Varela Rodriguez(HaZE).

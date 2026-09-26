# Associative Queue (SWAG) — Anima tu Estructura de Datos

Proyecto educativo para el curso **CS2023 Algoritmos y Estructuras de Datos (2026-2)** de la **Universidad de Ingeniería y Tecnología (UTEC)**.

Este proyecto implementa y anima una **Associative Queue (Sliding Window Aggregation - SWAG)** utilizando una implementación real en **C++20** y una animación programática generada con **Manim Community Edition (Python)**, siguiendo el estilo visual pedagógico de **3Blue1Brown**.

La animación se alimenta de una traza de ejecución (`trace.json`) generada por el programa en C++, garantizando que todas las operaciones mostradas correspondan al comportamiento real de la estructura de datos.

---

# Integrantes e Información Institucional

| Información     | Detalle                                                                                        |
| :-------------- | :--------------------------------------------------------------------------------------------- |
| **Institución** | Universidad de Ingeniería y Tecnología (UTEC)                                                  |
| **Curso**       | CS2023 Algoritmos y Estructuras de Datos                                                       |
| **Docente**     | Prof. Víctor Racsó Galván Oyola (`vgalvan@utec.edu.pe`)                                        |
| **Integrantes** | Huertos Ochoa, Rodrigo Franco • Ramos Vargas, Royer Sebastian Diego • Antonio Rosario Palomino |

---

# ¿Qué es una Associative Queue?

Una **Associative Queue** es una cola FIFO que, además de insertar al final y retirar del frente, permite consultar el resultado de aplicar una operación asociativa binaria **(⊕)** a sus elementos en orden **front → back**. La implementación con dos pilas acumulativas responde esa consulta en **O(1)**:

```math
\text{query}() = x_1 \oplus x_2 \oplus \dots \oplus x_n
```

### Propiedad de Asociatividad

La operación utilizada debe satisfacer la propiedad asociativa:

```math
(a \oplus b) \oplus c = a \oplus (b \oplus c)
```

| Operación                  | Asociativa |
| :------------------------- | :--------: |
| Suma (+)                   |     Sí     |
| Multiplicación (×)         |     Sí     |
| Máximo (max)               |     Sí     |
| Mínimo (min)               |     Sí     |
| Máximo Común Divisor (gcd) |     Sí     |

En esta implementación se utiliza la función de mínimo (**min**) como caso de estudio. Por ejemplo, para la cola `[8, 3, 5]`, `query()` devuelve `min(min(8, 3), 5) = 3`. Aunque `min` no depende del orden, la implementación combina los acumulados de salida antes que los de entrada para respetar el orden de la cola con otras operaciones asociativas.

---

# Funcionamiento Interno del SWAG

La cola se implementa mediante dos pilas de nodos enlazados, sin usar `std::stack` ni `std::queue`. Se usa `std::vector` únicamente para copiar los estados de las pilas que se exportan a `trace.json`; los elementos de la cola se almacenan en los nodos enlazados.

## Arquitectura Lógica

```text
                 Associative Queue
              ┌──────────────────────┐
              │                      │
              │   query() / fold()   │
              │                      │
              └──────────┬───────────┘
                         │
          ┌──────────────┴──────────────┐
          │                             │
     _stack_out                    _stack_in
    (Salida FIFO)                 (Entrada FIFO)

  Tope -> acumulado             Tope -> acumulado
```

## Pila de Entrada (`_stack_in`)

Recibe los nuevos elementos mediante `push(x)`. Cada nodo almacena su valor y el acumulado parcial calculado desde el fondo hasta dicho nodo:

```math
acc = acc_{anterior} \oplus valor
```

## Pila de Salida (`_stack_out`)

Desde donde se extraen los elementos mediante `pop()`. Cuando `_stack_out` queda vacía y se solicita una extracción, todos los elementos de `_stack_in` se transfieren invirtiendo el orden y recalculando los acumulados:

```math
acc = valor \oplus acc_{anterior}
```

## Consulta Global (`query()`)

Calcula el agregado en **O(1)** evaluando el estado de las pilas:

- Si solo `_stack_in` contiene datos: retorna el acumulado del tope de `_stack_in`.
- Si solo `_stack_out` contiene datos: retorna el acumulado del tope de `_stack_out`.
- Si ambas contienen datos: combina los topes precalculados en tiempo constante:

```math
resultado = tope(\_stack\_out).acc \oplus tope(\_stack\_in).acc
```

---

# Arquitectura del Sistema

```text
                IMPLEMENTACIÓN REAL

      Motor C++ (SWAG con nodos y punteros)
                     │
                     │ genera eventos
                     ▼
              trace.json (JSON)
                     │
                     │ leído por Python
                     ▼
          Manim Community Edition
                     │
                     │ renderiza
                     ▼
               Animación MP4
```

Los valores y los estados de las pilas mostrados por la animación provienen de `trace.json`, emitido por el motor de C++. Manim utiliza esos eventos para generar los movimientos y las explicaciones visuales.

---

# Estructura del Repositorio

```text
proyecto_01/
│
├── implementacion_cpp/
│   ├── AssociativeQueue.h      # Pila y cola asociativa implementadas con nodos enlazados
│   ├── TraceLogger.h           # Serializador de eventos a formato JSON
│   ├── main.cpp                # Simulación de operaciones y casos de prueba
│   └── CMakeLists.txt          # Configuración de compilación para CLion / CMake
│
├── animacion_manim/
│   ├── scenes.py               # Script de animación con Manim CE
│   ├── trace.json              # Registro de estados generado por C++
│   └── requirements.txt        # Dependencias de Python (manim)
│
├── .gitignore                  # Exclusiones de Git (builds, cache, temporales)
└── README.MD                   # Documentación técnica del proyecto
```

---

# Requisitos del Sistema

- **Compilador C++:** Compatible con **C++20** (`g++`, `clang++` o MSVC) y **CMake 4.0+**.
- **Python:** Versión **3.9 o superior**.
- **FFmpeg:** Requerido por Manim para la codificación y exportación de video.

### Windows (vía winget)

```bash
winget install Gyan.FFmpeg
```

### Linux (Debian/Ubuntu)

```bash
sudo apt install ffmpeg
```

### macOS (vía Homebrew)

```bash
brew install ffmpeg
```

---

# Instrucciones de Compilación y Ejecución

## 1. Generar la Traza de Ejecución (C++)

### Desde CLion

1. Abrir la carpeta `implementacion_cpp` en CLion.
2. Ir a **Run → Edit Configurations...**
3. Configurar **Working directory** con la ruta absoluta a la carpeta `implementacion_cpp`.
4. Ejecutar `main.cpp` (`Shift + F10`). El archivo `trace.json` se generará automáticamente en `animacion_manim/`.

### Desde Terminal (GCC/Clang)

```bash
cd implementacion_cpp

g++ -std=c++20 main.cpp -o engine

./engine ../animacion_manim/trace.json
```

## 2. Configurar el Entorno de Animación (Python)

Desde la carpeta `animacion_manim`:

```bash
cd ../animacion_manim

pip install -r requirements.txt
```

## 3. Renderizar la Animación con Manim

### Previsualización rápida (baja resolución, 480p a 15 fps)

```bash
manim -pql scenes.py AssociativeQueueScene
```

_(La bandera `-p` abre el reproductor predeterminado al terminar)._

### Renderizado final en alta definición

```bash
manim -pqh scenes.py AssociativeQueueScene
```

El video exportado se encontrará en:

```text
animacion_manim/media/videos/scenes/1080p60/AssociativeQueueScene.mp4
```

---

# Formato del Archivo `trace.json`

Los eventos registrados incluyen estados de la estructura para desacoplar el motor algorítmico del motor gráfico:

```json
{
  "step": 2,
  "op": "PUSH",
  "value": 8,
  "stack_in": [{ "val": 8, "acc": 8 }],
  "stack_out": [],
  "total_agg": 8,
  "desc": "Push(8): ingresa a _stack_in con acumulado 8"
}
```

---

# Análisis de Complejidad Temporal y Espacial

Las siguientes cotas describen **la estructura de datos**, sin contar la copia de estados para `trace.json` ni el renderizado de Manim.

| Operación | Complejidad                                                     | Justificación                                                                       |
| :-------- | :-------------------------------------------------------------- | :---------------------------------------------------------------------------------- |
| `push(x)` | **O(1)** por operación                                          | Inserta un nodo y calcula su acumulado usando el tope previo.                       |
| `pop()`   | **O(n)** en el peor caso de una extracción; **O(1) amortizado** | Si la pila de salida está vacía, transfiere los elementos desde la pila de entrada. |
| `query()` | **O(1)** por operación                                          | Consulta y combina, como máximo, los acumulados de los dos topes.                   |
| Espacio   | **O(n)**                                                        | Cada elemento ocupa un nodo con valor, acumulado y puntero.                         |

La generación de la traza añade trabajo: `to_vector()` recorre las pilas y `TraceLogger` almacena copias de sus estados. Por ello, las cotas anteriores **no describen el tiempo total de los métodos instrumentados** que generan el video.

## Justificación del Costo Amortizado de `pop()`

Una transferencia puede costar **O(n)**, pero cada elemento pasa de `_stack_in` a `_stack_out` **a lo sumo una vez** antes de salir de la cola. Así, el costo total de las transferencias se reparte entre las inserciones y extracciones de una secuencia: `pop()` tiene costo **O(1) amortizado**.

---

# Gestión de Memoria Dinámica

La estructura gestiona memoria dinámica explícita en el _heap_ de C++:

- Cada inserción asigna un nuevo nodo mediante `new StackNode(...)`.
- Cada extracción libera la memoria del nodo retirado mediante `delete temp`.
- La clase destructora `~AssociativeStack()` recorre y destruye los nodos remanentes al salir de alcance, liberando los nodos que permanezcan al salir de alcance.

---

# Referencias

1. [USACO Guide — Sliding Window, método de dos pilas](https://usaco.guide/gold/sliding-window). Referencia para agregados de ventana mediante pilas.
2. [Documentación oficial de Manim Community Edition](https://docs.manim.community/en/stable/). Herramienta utilizada para generar el video.
3. [3Blue1Brown](https://www.3blue1brown.com/). Referencia visual sugerida por la consigna.
4. Material de clase CS2023, semanas 3 (listas y memoria dinámica) y 4 (pilas y colas), UTEC 2026-2.

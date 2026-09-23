# Associative Queue (SWAG) — Anima tu Estructura de Datos

Proyecto educativo para el curso **CS2023 Algoritmos y Estructuras de Datos (2026-2)** de la **Universidad de Ingeniería y Tecnología (UTEC)**.

Este proyecto implementa y anima una **Associative Queue (Sliding Window Aggregation - SWAG)** utilizando una implementación real en **C++17** y una animación programática generada con **Manim Community Edition (Python)**, siguiendo el estilo visual pedagógico de **3Blue1Brown**.

La animación se alimenta de una traza de ejecución (`trace.json`) generada por el programa en C++, garantizando que todas las operaciones mostradas correspondan al comportamiento real de la estructura de datos.

---

# Integrantes e Información Institucional

| Información     | Detalle                                                          |
| :-------------- | :--------------------------------------------------------------- |
| **Institución** | Universidad de Ingeniería y Tecnología (UTEC)                    |
| **Curso**       | CS2023 Algoritmos y Estructuras de Datos[cite: 1]                |
| **Docente**     | Prof. Víctor Racsó Galván Oyola (`vgalvan@utec.edu.pe`)[cite: 1] |
| **Integrantes** | Huertos Ochoa, Rodrigo Franco • Ramos Vargas, Royer Sebastian    |

---

# ¿Qué es una Associative Queue?

Una **Associative Queue** es una cola FIFO que permite consultar en tiempo constante **O(1)** el resultado de aplicar una operación asociativa binaria **(⊕)** sobre todos los elementos actualmente almacenados:

```math
\text{query}() = x_1 \oplus x_2 \oplus \dots \oplus x_n
```

### Propiedad de Asociatividad

La operación utilizada debe satisfacer la propiedad asociativa[cite: 1]:

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

En esta implementación se utiliza la función de mínimo (**min**) como caso de estudio demostrativo.

---

# Funcionamiento Interno del SWAG

La estructura se implementa mediante **dos pilas enlazadas dinámicamente**, sin utilizar `std::stack`, `std::queue` ni `std::vector`, respetando la gestión manual de memoria enlazada[cite: 1, 7, 8].

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

Recibe los nuevos elementos mediante `push(x)`. Cada nodo almacena su valor y el acumulado parcial calculado desde el fondo hasta dicho nodo[cite: 7]:

```math
acc = acc_{anterior} \oplus valor
```

## Pila de Salida (`_stack_out`)

Desde donde se extraen los elementos mediante `pop()`. Cuando `_stack_out` queda vacía y se solicita una extracción, todos los elementos de `_stack_in` se transfieren invirtiendo el orden y recalculando los acumulados[cite: 7]:

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

La animación **no simula valores a mano**: cada transformación visual proviene estrictamente del archivo `trace.json` emitido por el motor de C++[cite: 1].

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
├── animacion_video/            # Directorio destino del video final exportado
├── .gitignore                  # Exclusiones de Git (builds, cache, temporales)
└── README.md                   # Documentación técnica del proyecto
```

---

# Requisitos del Sistema

- **Compilador C++:** Compatible con **C++17** (`g++`, `clang++` o MSVC) y **CMake 3.20+**.
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

g++ -std=c++17 main.cpp -o engine

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

### Renderizado final en alta definición (1080p a 60 fps)

```bash
manim -pqh scenes.py AssociativeQueueScene
```

El video exportado se encontrará en:

```text
animacion_manim/media/videos/scenes/1080p60/AssociativeQueueScene.mp4
```

---

# Formato del Archivo `trace.json`

Cada operación genera un estado completo de la estructura para desacoplar el motor algorítmico del motor gráfico[cite: 1, 6]:

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

| Operación            | Complejidad         | Justificación                                                                                                                           |
| :------------------- | :------------------ | :-------------------------------------------------------------------------------------------------------------------------------------- |
| `push(x)`            | **O(1)** amortizado | Crea un nodo al tope de `_stack_in` y calcula el acumulado con el nodo previo en tiempo constante.[cite: 7]                             |
| `pop()`              | **O(1)** amortizado | Retira el nodo del tope de `_stack_out` en O(1). La transferencia de **N** elementos se distribuye uniformemente en el tiempo.[cite: 7] |
| `query()` / `fold()` | **O(1)** estricto   | Combina a lo sumo los acumulados ya calculados en los topes de ambas pilas[cite: 1].                                                    |
| Espacio de memoria   | **O(N)**            | Cada elemento ocupa un nodo dinámico con campos `val`, `acc` y puntero `next`.[cite: 7]                                                 |

---

## Justificación del Costo Amortizado

Aunque la transferencia entre pilas toma tiempo **O(N)** en el peor caso puntual, cada elemento insertado experimenta exactamente tres operaciones a lo largo de su ciclo de vida:

1. Inserción en `_stack_in` (**O(1)**).
2. Transferencia única a `_stack_out` (**O(1)**).
3. Eliminación final de `_stack_out` (**O(1)**).

El costo promedio por operación tras una secuencia de **N** operaciones es **O(1)**[cite: 1, 7].

---

# Gestión de Memoria Dinámica

La estructura gestiona memoria dinámica explícita en el _Heap_[cite: 7]:

- Cada inserción asigna un nuevo nodo mediante `new StackNode(...)`.
- Cada extracción libera la memoria del nodo retirado mediante `delete temp`.
- La clase destructora `~AssociativeStack()` recorre y destruye los nodos remanentes al salir de alcance, previniendo fugas de memoria (_memory leaks_).

---

# Referencias

1. CP-Algorithms — _Sliding Window Aggregation (SWAG)_.
2. Manim Community Edition Documentation (`docs.manim.community`)[cite: 1].
3. 3Blue1Brown — Referencia de diseño y animación pedagógica[cite: 1].
4. Material de clase CS2023: Semana 3 (Listas y Memoria Dinámica) y Semana 4 (Pilas y Colas), Prof. Víctor Racsó Galván Oyola, UTEC 2026-2.

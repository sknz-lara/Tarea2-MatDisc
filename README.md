# 🏰 Proyecto Castillo
**Asignatura:** <Natenaticas Discretas>  
**Profesor/a:** <Guillermo >  
**Integrantes:** <Martín Lara Castro> | <Valentina Serón Canales> | <Nicolás Torres Burgos>
**Fecha:** <03/11/2025>  

---

## 🧩 Descripción general
Este programa construye y analiza un **castillo representado como grafo tridimensional**, donde cada celda puede contener muros, portales, escaleras o monstruos.  
El objetivo es encontrar un **camino desde la posición de Hrongan hasta la salida**, considerando las **vidas iniciales** y los **costos por monstruos**.

Durante la ejecución, el usuario:
1. Ingresa el nombre del archivo del castillo.  
2. Ingresa la cantidad de vidas iniciales.  
3. El programa calcula estadísticas del grafo y ejecuta una búsqueda BFS para encontrar una ruta hacia la salida.

---

## ⚙️ Requisitos
- **Compilador:** `gcc` compatible con estándar **C11**  
- **Sistema operativo:** Linux o Windows con **WSL**  
- **Herramientas necesarias:**  
  ```bash
  sudo apt install build-essential
  ```

---

## 🧱 Compilación

### Opción 1 — Usando Makefile (recomendada)
Compilar el proyecto:
```bash
make
```

Ejecutar el programa:
```bash
make run
```

Limpiar archivos intermedios:
```bash
make clean
```

Reconstruir desde cero:
```bash
make re
```

> El ejecutable generado se llama **`castillo`**.

---

### Opción 2 — Compilación manual con `gcc`
También puedes compilar el programa directamente:
```bash
gcc -std=c11 -Wall -Wextra -O2 main.c castle.c draft.c reader.c bfs.c -o castillo
```

Ejecutar:
```bash
./castillo
```

---

## ▶️ Uso del programa

1. Ejecuta el binario:
   ```bash
   ./castillo
   ```
2. Ingresa el **nombre del archivo** del castillo (por ejemplo: `castillo1.txt`).
3. Ingresa el número de **vidas iniciales** (entero positivo).
4. El programa mostrará:
   - Información general del castillo (pisos, dimensiones, posición de Hrongan y de la salida).  
   - Cantidad total de **aristas** y **promedio de vecinos por nodo**.  
   - Resultado del **camino calculado** mediante BFS según las vidas disponibles.
5. Al finalizar, podrás elegir si deseas procesar otro archivo (`y/n`).

---

## 📁 Estructura del proyecto

| Archivo        | Descripción breve |
|----------------|-------------------|
| `main.c`       | Lógica principal del programa y bucle de interacción con el usuario |
| `reader.c/h`   | Lectura y validación del archivo del castillo |
| `castle.c/h`   | Definición del grafo y funciones para agregar muros, portales, enemigos, etc. |
| `bfs.c/h`      | Implementación de la búsqueda BFS para encontrar rutas |
| `draft.c/h`    | Renderizado ASCII adicional del castillo (depuración o visualización) |
| `Makefile`     | Script de compilación y automatización |

---

## 🧰 Notas adicionales
- El programa valida todas las entradas del usuario (incluyendo errores de lectura o entradas vacías).
- Si el archivo no se puede leer o tiene formato inválido, el programa indicará el error y permitirá volver a intentar.
---

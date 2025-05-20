# Banquero: Detección de Interbloqueos (Deadlocks)

“Banquero” es un programa en C que implementa el algoritmo del Banquero para determinar si un sistema de procesos y recursos se halla en un estado seguro (sin riesgo de deadlock) o en un estado inseguro (posible interbloqueo). En caso de estado seguro, también muestra una secuencia válida de ejecución de procesos.

---

## Requisitos

- **Compilador C** compatible con C11 o superior (GCC, Clang, etc.).
- Entorno Unix/Linux o Windows (MinGW/Cygwin).
- `make` (opcional, si deseas usar un Makefile).

---

## Estructura del proyecto
```
├── banquero.c    # Código fuente principal
├── datos.txt     # Archivo de entrada con datos de procesos y recursos
├── README.md     # Documentación del proyecto
├── makefile      # Makefile para compilar el programa
```
## Compilación

1. **Sin Makefile**  
Desde el directorio del proyecto, ejecuta:
```bash
  gcc -Wall -Wextra -O2 banquero.c -o banquero
 ```
2. **Con Makefile**
Si tienes un `makefile`, simplemente ejecuta:
```bash
  make
```
Esto generará un ejecutable llamado `banquero`.
## Uso
```bash
  ./banquero --help
```
Esto mostrará la ayuda del programa, incluyendo cómo usarlo y los parámetros necesarios.

```bash
  ./banquero <archivo>
```
Lee la descripción de procesos y recursos desde `<archivo>` y determina si el sistema está en estado seguro o detecta un posible deadlock.

## Recursos
- [Wikipedia contributors. (n.d.). *Algoritmo del banquero*](https://es.wikipedia.org/wiki/Algoritmo_del_banquero)

- [Dijkstra, E. W. (1968). *Co‑operating sequential processes*. En F. Genuys (Ed.), *Programming languages: NATO Advanced Study Institute: Lectures given at a three weeks Summer School held in Villard‑le‑Lans, 1966* (pp. 43–112). Academic Press Inc.](https://pure.tue.nl/ws/files/4279816/344354178746665.pdf)

- [GeeksforGeeks. (n.d.). *Banker’s Algorithm in Operating System*](https://www.geeksforgeeks.org/bankers-algorithm-in-operating-system/)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COLOR_RESET    "\x1b[0m"
#define COLOR_BOLD     "\x1b[1m"
#define COLOR_RED      "\x1b[31m"
#define COLOR_GREEN    "\x1b[32m"
#define COLOR_YELLOW   "\x1b[33m"
#define COLOR_BLUE     "\x1b[34m"
#define COLOR_CYAN     "\x1b[36m"

void print_vector_int(const char *nombre, int *v, int len) {
    printf(COLOR_CYAN "%s = [" COLOR_RESET, nombre);
    for (int i = 0; i < len; i++) {
        printf("%d", v[i]);
        if (i < len - 1) printf(" ");
    }
    printf("]\n");
}

void print_help(void) {
    printf(COLOR_BOLD COLOR_BLUE "BANQUERO: Detección de Interbloqueos (Algoritmo del Banquero)\n\n" COLOR_RESET);

    printf(COLOR_YELLOW "USO:\n" COLOR_RESET);
    printf("  banquero --help\n");
    printf("      Muestra esta ayuda.\n");
    printf("  banquero <archivo_entrada>\n");
    printf("      Comprueba si el sistema está en estado seguro a partir de los datos\n");
    printf("      de recursos y procesos descritos en el fichero.\n\n");

    printf(COLOR_YELLOW "FORMATO DEL ARCHIVO DE ENTRADA:\n" COLOR_RESET);
    printf("  1) Primera línea:\n");
    printf("       n m\n");
    printf("     – n = número de procesos (entero > 0)\n");
    printf("     – m = número de tipos de recurso (entero > 0)\n\n");

    printf("  2) Segunda línea (m valores):\n");
    printf("       Available[0] Available[1] … Available[m−1]\n");
    printf("     – Cada valor indica cuántas unidades libres hay de ese recurso.\n\n");

    printf("  3) Siguientes n líneas (matriz Asignacion):\n");
    printf("       Asignacion[i][0] Asignacion[i][1] … Asignacion[i][m−1]\n");
    printf("     – Cuántas unidades de cada recurso tiene asignado el proceso i.\n\n");

    printf("  4) Últimas n líneas (matriz Demanda):\n");
    printf("       Demanda[i][0] Demanda[i][1] … Demanda[i][m−1]\n");
    printf("     – Demanda máxima declarada por el proceso i para cada recurso.\n\n");

    printf("   – Internamente se calcula Necesidad[i][j] = Demanda[i][j] – Asignacion[i][j].\n\n");

    printf(COLOR_YELLOW "SALIDA:\n" COLOR_RESET);
    printf("  • Si el estado es seguro:\n");
    printf("      " COLOR_GREEN "El sistema está en ESTADO SEGURO." COLOR_RESET "\n");
    printf("      Secuencia segura: P<índice> → P<índice> → …\n\n");

    printf("  • Si el estado es inseguro o hay deadlock:\n");
    printf("      " COLOR_RED "¡INTERBLOQUEO DETECTADO! No existe secuencia segura." COLOR_RESET "\n\n");

    printf(COLOR_YELLOW "EJEMPLO DE ARCHIVO “datos.txt”:\n" COLOR_RESET);
    printf("  5 3\n");
    printf("  9 5 6           # Existencias (Available)\n");
    printf("  0 1 0           # Asignacion P1\n");
    printf("  2 0 0           # Asignacion P2\n");
    printf("  3 0 2           # Asignacion P3\n");
    printf("  2 1 1           # Asignacion P4\n");
    printf("  0 0 2           # Asignacion P5\n");
    printf("  2 3 1           # Demanda P1\n");
    printf("  3 2 2           # Demanda P2\n");
    printf("  9 0 2           # Demanda P3\n");
    printf("  2 5 2           # Demanda P4\n");
    printf("  4 3 3           # Demanda P5\n\n");

    printf("  – 5 procesos (P1..P5), 3 recursos (R1..R3).\n\n");

    printf(COLOR_YELLOW "EJEMPLO DE INVOCACIÓN:\n" COLOR_RESET);
    printf("  $ ./banquero --help\n");
    printf("  $ ./banquero datos.txt\n");
}

bool is_help_arg(char *arg) {
    return (strcmp(arg, "uso") == 0 ||
            strcmp(arg, "help") == 0 ||
            strcmp(arg, "-h") == 0 ||
            strcmp(arg, "--help") == 0);
}

int main(int argc, char *argv[]) {
    if (argc == 1 || (argc == 2 && is_help_arg(argv[1]))) {
        print_help();
        return 0;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, COLOR_RED "Error abriendo el archivo: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }

    int n, m;
    if (fscanf(fp, "%d %d", &n, &m) != 2) {
        fprintf(stderr, COLOR_RED "Formato inválido en la primera línea.\n" COLOR_RESET);
        fclose(fp);
        return 1;
    }
    if (n <= 0 || m <= 0) {
        fprintf(stderr, COLOR_RED "Error: n y m deben ser mayores que cero.\n" COLOR_RESET);
        fclose(fp);
        return 1;
    }

    int *Existencias           = (int*)malloc(m * sizeof(int));
    int *RecursosUsados        = (int*)malloc(m * sizeof(int));
    int *RecursosDisponibles   = (int*)malloc(m * sizeof(int));
    int *Asignacion            = (int*)malloc(n * m * sizeof(int));
    int *Demanda               = (int*)malloc(n * m * sizeof(int));
    int *Necesidad             = (int*)malloc(n * m * sizeof(int));

    if (!Existencias || !RecursosUsados || !RecursosDisponibles ||
        !Asignacion  || !Demanda        || !Necesidad) {
        fprintf(stderr, COLOR_RED "Error de asignación de memoria.\n" COLOR_RESET);
        fclose(fp);
        return 1;
    }

    for (int j = 0; j < m; j++) {
        if (fscanf(fp, "%d", &Existencias[j]) != 1) {
            fprintf(stderr, COLOR_RED "Error leyendo Existencias[%d].\n" COLOR_RESET, j);
            fclose(fp);
            return 1;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fscanf(fp, "%d", &Asignacion[i*m + j]) != 1) {
                fprintf(stderr, COLOR_RED "Error leyendo Asignacion[%d][%d].\n" COLOR_RESET, i, j);
                fclose(fp);
                return 1;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fscanf(fp, "%d", &Demanda[i*m + j]) != 1) {
                fprintf(stderr, COLOR_RED "Error leyendo Demanda[%d][%d].\n" COLOR_RESET, i, j);
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);

    printf(COLOR_BLUE "\n--- Cálculo de la matriz Necesidad (Max – Allocation) ---\n\n" COLOR_RESET);
    for (int i = 0; i < n; i++) {
        printf(COLOR_YELLOW "P%d:" COLOR_RESET " Demanda = [", i + 1);
        for (int j = 0; j < m; j++) {
            printf("%d", Demanda[i*m + j]);
            if (j < m - 1) printf(" ");
        }
        printf("], Asignación = [");
        for (int j = 0; j < m; j++) {
            printf("%d", Asignacion[i*m + j]);
            if (j < m - 1) printf(" ");
        }
        printf("]  →  Necesidad = [");
        for (int j = 0; j < m; j++) {
            Necesidad[i*m + j] = Demanda[i*m + j] - Asignacion[i*m + j];
            if (Necesidad[i*m + j] < 0) {
                fprintf(stderr, COLOR_RED "\nError: Necesidad[%d][%d] < 0.\n" COLOR_RESET, i, j);
                free(Existencias);
                free(RecursosUsados);
                free(RecursosDisponibles);
                free(Asignacion);
                free(Demanda);
                free(Necesidad);
                return 1;
            }
            printf("%d", Necesidad[i*m + j]);
            if (j < m - 1) printf(" ");
        }
        printf("]\n");
    }

    for (int j = 0; j < m; j++) {
        RecursosUsados[j] = 0;
        for (int i = 0; i < n; i++) {
            RecursosUsados[j] += Asignacion[i*m + j];
        }
    }
    print_vector_int("\nRecursosUsados", RecursosUsados, m);

    for (int j = 0; j < m; j++) {
        RecursosDisponibles[j] = Existencias[j] - RecursosUsados[j];
        if (RecursosDisponibles[j] < 0) {
            fprintf(stderr, COLOR_RED "Error: RecursosDisponibles[%d] es negativo.\n" COLOR_RESET, j);
            free(Existencias);
            free(RecursosUsados);
            free(RecursosDisponibles);
            free(Asignacion);
            free(Demanda);
            free(Necesidad);
            return 1;
        }
    }
    print_vector_int(COLOR_YELLOW "\nExistencias (Available)" COLOR_RESET, Existencias, m);
    print_vector_int(COLOR_YELLOW "RecursosDisponibles (inicial)" COLOR_RESET, RecursosDisponibles, m);

    bool *finalizado = (bool*)malloc(n * sizeof(bool));
    int  *secuencia  = (int*)malloc(n * sizeof(int));
    if (!finalizado || !secuencia) {
        fprintf(stderr, COLOR_RED "Error de asignación de memoria.\n" COLOR_RESET);
        return 1;
    }
    for (int i = 0; i < n; i++) {
        finalizado[i] = false;
    }

    printf(COLOR_BLUE "\n--- Inicio del Algoritmo del Banquero (trazado paso a paso) ---\n\n" COLOR_RESET);

    int count = 0;
    bool progreso = true;

    while (count < n && progreso) {
        progreso = false;
        print_vector_int(COLOR_YELLOW "\nRecursosDisponibles (antes de esta ronda)" COLOR_RESET, RecursosDisponibles, m);

        for (int i = 0; i < n; i++) {
            if (!finalizado[i]) {
                printf(COLOR_CYAN "\nIntentando verificar P%d:\n" COLOR_RESET, i + 1);
                printf("  Necesidad P%d = [", i + 1);
                for (int j = 0; j < m; j++) {
                    printf("%d", Necesidad[i*m + j]);
                    if (j < m - 1) printf(" ");
                }
                printf("]\n");
                printf("  RecursosDisponibles actuales = [");
                for (int j = 0; j < m; j++) {
                    printf("%d", RecursosDisponibles[j]);
                    if (j < m - 1) printf(" ");
                }
                printf("]\n");

                bool puede_ejecutar = true;
                for (int j = 0; j < m; j++) {
                    if (Necesidad[i*m + j] > RecursosDisponibles[j]) {
                        printf("  -> " COLOR_RED "NO se cumple:" COLOR_RESET " Necesidad P%d[%d] = %d > Disponibles[%d] = %d\n",
                               i + 1,
                               j,
                               Necesidad[i*m + j],
                               j,
                               RecursosDisponibles[j]);
                        puede_ejecutar = false;
                        break;
                    }
                }
                if (puede_ejecutar) {
                    printf("  -> " COLOR_GREEN "P%d puede ejecutarse (Need ≤ Disponibles)." COLOR_RESET "\n", i + 1);
                    printf("  → " COLOR_YELLOW "Ejecutando P%d y liberando Asignación P%d = [" COLOR_RESET, i + 1, i + 1);
                    for (int j = 0; j < m; j++) {
                        printf("%d", Asignacion[i*m + j]);
                        if (j < m - 1) printf(" ");
                    }
                    printf("]" COLOR_RESET "\n");

                    for (int j = 0; j < m; j++) {
                        RecursosDisponibles[j] += Asignacion[i*m + j];
                    }
                    finalizado[i] = true;
                    secuencia[count++] = i;

                    printf("  → " COLOR_CYAN "RecursosDisponibles actualizados = [" COLOR_RESET);
                    for (int j = 0; j < m; j++) {
                        printf("%d", RecursosDisponibles[j]);
                        if (j < m - 1) printf(" ");
                    }
                    printf("]" COLOR_RESET "\n");

                    progreso = true;
                    break;
                }
            }
        }

        if (!progreso) {
            printf(COLOR_RED "\n  No se encontró ningún proceso que pueda ejecutarse en esta ronda.\n" COLOR_RESET);
        }
    }

    printf(COLOR_BLUE "\n--- Fin del Algoritmo del Banquero ---\n\n" COLOR_RESET);

    if (count == n) {
        printf(COLOR_GREEN "El sistema está en ESTADO SEGURO.\n" COLOR_RESET);
        printf(COLOR_YELLOW "Secuencia segura: " COLOR_RESET);
        for (int k = 0; k < n; k++) {
            printf(COLOR_BOLD "P%d" COLOR_RESET, secuencia[k] + 1);
            if (k < n - 1) printf(" → ");
        }
        printf("\n");
    } else {
        printf(COLOR_RED "¡INTERBLOQUEO DETECTADO! No existe secuencia segura.\n" COLOR_RESET);
    }

    free(Existencias);
    free(RecursosUsados);
    free(RecursosDisponibles);
    free(Asignacion);
    free(Demanda);
    free(Necesidad);
    free(finalizado);
    free(secuencia);

    return 0;
}
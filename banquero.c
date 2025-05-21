#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void print_vector_int(const char *nombre, int *v, int len) {
    printf("%s = [", nombre);
    for (int i = 0; i < len; i++) {
        printf("%d", v[i]);
        if (i < len - 1) printf(" ");
    }
    printf("]\n");
}

void print_help(void) {
    printf("BANQUERO: Detección de Interbloqueos (Algoritmo del Banquero)\n\n");

    printf("USO:\n");
    printf("  banquero --help\n");
    printf("      Muestra esta ayuda.\n");
    printf("  banquero <archivo_entrada>\n");
    printf("      Comprueba si el sistema está en estado seguro a partir de los datos\n");
    printf("      de recursos y procesos descriptos en el fichero.\n\n");

    printf("FORMATO DEL ARCHIVO DE ENTRADA:\n");
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

    printf("SALIDA:\n");
    printf("  • Si el estado es seguro:\n");
    printf("      El sistema está en ESTADO SEGURO.\n");
    printf("      Secuencia segura: P<índice> → P<índice> → …\n\n");

    printf("  • Si el estado es inseguro o hay deadlock:\n");
    printf("      ¡INTERBLOQUEO DETECTADO! No existe secuencia segura.\n\n");

    printf("EJEMPLO DE ARCHIVO “datos.txt”:\n");
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

    printf("EJEMPLO DE INVOCACIÓN:\n");
    printf("  $ ./banquero --help\n");
    printf("  $ ./banquero datos.txt\n");
}

bool help(char *arg) {
    return (strcmp(arg, "uso") == 0 ||
            strcmp(arg, "help") == 0 ||
            strcmp(arg, "-h") == 0 ||
            strcmp(arg, "--help") == 0);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_help();
        return 0;
    }

    if (argc == 2 && help(argv[1])) {
        print_help();
        return 0;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error abriendo el archivo");
        return 1;
    }

    int n, m;
    if (fscanf(fp, "%d %d", &n, &m) != 2) {
        fprintf(stderr, "Formato inválido en la primera línea.\n");
        fclose(fp);
        return 1;
    }
    if (n <= 0 || m <= 0) {
        fprintf(stderr, "n y m deben ser mayores que cero.\n");
        fclose(fp);
        return 1;
    }

    int *Existencias       = malloc(m * sizeof(int));
    int *RecursosUsados    = malloc(m * sizeof(int));
    int *RecursosDisponibles = malloc(m * sizeof(int));
    int *Asignacion        = malloc(n * m * sizeof(int));
    int *Demanda           = malloc(n * m * sizeof(int));
    int *Necesidad         = malloc(n * m * sizeof(int));

    if (!Existencias || !RecursosUsados || !RecursosDisponibles ||
        !Asignacion || !Demanda || !Necesidad) {
        fprintf(stderr, "Error de asignación de memoria.\n");
        fclose(fp);
        return 1;
    }

    for (int j = 0; j < m; j++) {
        if (fscanf(fp, "%d", &Existencias[j]) != 1) {
            fprintf(stderr, "Error leyendo Existencias[%d].\n", j);
            fclose(fp);
            return 1;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fscanf(fp, "%d", &Asignacion[i*m + j]) != 1) {
                fprintf(stderr, "Error leyendo Asignacion[%d][%d].\n", i, j);
                fclose(fp);
                return 1;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fscanf(fp, "%d", &Demanda[i*m + j]) != 1) {
                fprintf(stderr, "Error leyendo Demanda[%d][%d].\n", i, j);
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);

    printf("\n--- Cálculo de la matriz Necesidad (Max – Allocation) ---\n");
    for (int i = 0; i < n; i++) {
        printf("P%d: ", i + 1);
        printf("Demanda = [");
        for (int j = 0; j < m; j++) {
            printf("%d", Demanda[i*m + j]);
            if (j < m - 1) printf(" ");
        }
        printf("], Asignacion = [");
        for (int j = 0; j < m; j++) {
            printf("%d", Asignacion[i*m + j]);
            if (j < m - 1) printf(" ");
        }
        printf("]  →  Necesidad = [");
        for (int j = 0; j < m; j++) {
            Necesidad[i*m + j] = Demanda[i*m + j] - Asignacion[i*m + j];
            printf("%d", Necesidad[i*m + j]);
            if (j < m - 1) printf(" ");
        }
        printf("]\n");
        if (Necesidad[i*m + 0] < 0) {
            fprintf(stderr,"Error: Necesidad[%d][0] < 0.\n", i);
            return 1;
        }
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
            fprintf(stderr, "Error: RecursosDisponibles[%d] es negativo.\n", j);
            return 1;
        }
    }
    print_vector_int("Existencias (Available)", Existencias, m);
    print_vector_int("RecursosDisponibles (initial)", RecursosDisponibles, m);

    bool *finalizado = malloc(n * sizeof(bool));
    int  *secuencia  = malloc(n * sizeof(int));
    if (!finalizado || !secuencia) {
        fprintf(stderr, "Error de asignación de memoria.\n");
        return 1;
    }
    for (int i = 0; i < n; i++) {
        finalizado[i] = false;
    }

    printf("\n--- Inicio del Algoritmo del Banquero (trazado paso a paso) ---\n");

    int count = 0;
    bool progreso = true;

    while (count < n && progreso) {
        progreso = false;

        print_vector_int("\nRecursosDisponibles (antes de esta ronda)", RecursosDisponibles, m);

        for (int i = 0; i < n; i++) {
            if (!finalizado[i]) {
                printf("\nIntentando verificar P%d:\n", i + 1);
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
                        printf("  -> NO se cumple: Necesidad P%d[%d] = %d > "
                               "Disponibles[%d] = %d\n",
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
                    printf("  -> P%d puede ejecutarse (Need ≤ Disponibles).\n", i + 1);
                    printf("  → Ejecutando P%d y liberando Asignacion P%d = [", i + 1, i + 1);
                    for (int j = 0; j < m; j++) {
                        printf("%d", Asignacion[i*m + j]);
                        if (j < m - 1) printf(" ");
                    }
                    printf("]\n");

                    for (int j = 0; j < m; j++) {
                        RecursosDisponibles[j] += Asignacion[i*m + j];
                    }
                    finalizado[i] = true;
                    secuencia[count++] = i;

                    printf("  → RecursosDisponibles actualizados = [");
                    for (int j = 0; j < m; j++) {
                        printf("%d", RecursosDisponibles[j]);
                        if (j < m - 1) printf(" ");
                    }
                    printf("]\n");

                    progreso = true;
                    break;
                }
            }
        }

        if (!progreso) {
            printf("\n  No se encontró ningún proceso que pueda ejecutarse en esta ronda.\n");
        }
    }

    printf("\n--- Fin del Algoritmo del Banquero ---\n");

    if (count == n) {
        printf("\nEl sistema está en ESTADO SEGURO.\n");
        printf("Secuencia segura: ");
        for (int k = 0; k < n; k++) {
            printf("P%d", secuencia[k] + 1);
            if (k < n - 1) printf(" → ");
        }
        printf("\n");
    } else {
        printf("\n¡INTERBLOQUEO DETECTADO! No existe secuencia segura.\n");
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

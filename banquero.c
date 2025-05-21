#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

    printf("  3) Siguientes n líneas (matriz Allocation):\n");
    printf("       Allocation[i][0] Allocation[i][1] … Allocation[i][m−1]\n");
    printf("     – Cuántas unidades de cada recurso tiene asignado el proceso i.\n\n");

    printf("  4) Últimas n líneas (matriz Max):\n");
    printf("       Max[i][0] Max[i][1] … Max[i][m−1]\n");
    printf("     – Demanda máxima declarada por el proceso i para cada recurso.\n\n");

    printf("   – Internamente se calcula Need[i][j] = Max[i][j] – Allocation[i][j].\n\n");

    printf("SALIDA:\n");
    printf("  • Si el estado es seguro:\n");
    printf("      El sistema está en ESTADO SEGURO.\n");
    printf("      Secuencia segura: P<índice> → P<índice> → …\n\n");

    printf("  • Si el estado es inseguro o hay deadlock:\n");
    printf("      ¡INTERBLOQUEO DETECTADO! No existe secuencia segura.\n\n");

    printf("EJEMPLO DE ARCHIVO “datos.txt”:\n");
    printf("  5 3\n");
    printf("  9 5 6           # Existencias (Available)\n");
    printf("  0 1 0           # Allocation P1\n");
    printf("  2 0 0           # Allocation P2\n");
    printf("  3 0 2           # Allocation P3\n");
    printf("  2 1 1           # Allocation P4\n");
    printf("  0 0 2           # Allocation P5\n");
    printf("  2 3 1           # Max (Demanda) P1\n");
    printf("  3 2 2           # Max (Demanda) P2\n");
    printf("  9 0 2           # Max (Demanda) P3\n");
    printf("  2 5 2           # Max (Demanda) P4\n");
    printf("  4 3 3           # Max (Demanda) P5\n\n");

    printf("  – 5 procesos (P1..P5), 3 recursos (R1..R3).\n\n");

    printf("EJEMPLO DE INVOCACIÓN:\n");
    printf("  $ ./banquero --help\n");
    printf("  $ ./banquero datos.txt\n");
}

bool help (char *arg) {
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

    int *Existencias = malloc(m * sizeof(int));
    int *RecursosUsados = malloc(m * sizeof(int));
    int *RecursosDisponibles = malloc(m * sizeof(int));
    int *Allocation = malloc(n * m * sizeof(int));
    int *Max       = malloc(n * m * sizeof(int));
    int *Need      = malloc(n * m * sizeof(int));

    if (!Existencias || !RecursosUsados || !RecursosDisponibles ||
        !Allocation || !Max || !Need) {
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
            if (fscanf(fp, "%d", &Allocation[i*m + j]) != 1) {
                fprintf(stderr, "Error leyendo Allocation[%d][%d].\n", i, j);
                fclose(fp);
                return 1;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fscanf(fp, "%d", &Max[i*m + j]) != 1) {
                fprintf(stderr, "Error leyendo Max[%d][%d].\n", i, j);
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            Need[i*m + j] = Max[i*m + j] - Allocation[i*m + j];
            if (Need[i*m + j] < 0) {
                fprintf(stderr,
                        "Error: Need[%d][%d] = Max – Allocation es negativo.\n",
                        i, j);
                return 1;
            }
        }
    }

    for (int j = 0; j < m; j++) {
        RecursosUsados[j] = 0;
        for (int i = 0; i < n; i++) {
            RecursosUsados[j] += Allocation[i*m + j];
        }
    }

    for (int j = 0; j < m; j++) {
        RecursosDisponibles[j] = Existencias[j] - RecursosUsados[j];
        if (RecursosDisponibles[j] < 0) {
            fprintf(stderr,
                    "Error: RecursosDisponibles[%d] es negativo.\n", j);
            return 1;
        }
    }

    bool *finalizado = malloc(n * sizeof(bool));
    int  *secuencia  = malloc(n * sizeof(int));
    if (!finalizado || !secuencia) {
        fprintf(stderr, "Error de asignación de memoria.\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        finalizado[i] = false;
    }

    int count = 0;
    bool progreso = true;

    while (count < n && progreso) {
    progreso = false;
    for (int i = 0; i < n; i++) {
        if (!finalizado[i]) {
            bool puede_ejecutar = true;
            for (int j = 0; j < m; j++) {
                if (Need[i*m + j] > RecursosDisponibles[j]) {
                    puede_ejecutar = false;
                    break;
                }
            }
            if (puede_ejecutar) {
                for (int j = 0; j < m; j++) {
                    RecursosDisponibles[j] += Allocation[i*m + j];
                }
                finalizado[i]   = true;
                secuencia[count++] = i;
                progreso        = true;
                break;
            }
        }
    }
    }

    if (count == n) {
        printf("El sistema está en ESTADO SEGURO.\n");
        printf("Secuencia segura: ");
        for (int k = 0; k < n; k++) {
            printf("P%d", secuencia[k] + 1);
            if (k < n - 1) printf(" → ");
        }
        printf("\n");
    } else {
        printf("¡INTERBLOQUEO DETECTADO! No existe secuencia segura.\n");
    }

    free(Existencias);
    free(RecursosUsados);
    free(RecursosDisponibles);
    free(Allocation);
    free(Max);
    free(Need);
    free(finalizado);
    free(secuencia);

    return 0;
}

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
    printf("  4 3\n");
    printf("  10 5 7\n");
    printf("  0 1 0\n");
    printf("  2 0 0\n");
    printf("  3 0 2\n");
    printf("  2 1 1\n");
    printf("  7 5 3\n");
    printf("  3 2 2\n");
    printf("  9 0 2\n");
    printf("  4 2 2\n\n");

    printf("  – 4 procesos (P0..P3), 3 recursos (R0..R2).\n\n");

    printf("EJEMPLO DE INVOCACIÓN:\n");
    printf("  $ ./banquero --help\n");
    printf("  $ ./banquero datos.txt\n");
}

bool help (char *arg) {
  if (strcmp(arg, "uso") == 0 || strcmp(arg, "help") == 0 || strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
    return true;
  }
  return false;
}

int main(int argc, char *argv[]){
  int n,m;
  int Existencias [m];
  int Demanda[n][m];
  int Asignacion[n][m];
  int Necesidad[n][m];
  
  FILE *fp;
  if (argc == 1){
    print_help();
    return 0;
  } else if (argc == 2){
    if (help(argv[1])) {
      print_help();
      return 0;
    } else {
      fp = fopen(argv[1], "r");
    if (fp == NULL) {
      perror("Error opening file");
      return 1;
    }
    fscanf(fp, "%d %d", &n, &m);
    for (int i = 0; i < m; i++) {
      fscanf(fp, "%d", &Existencias[i]);
    }
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        fscanf(fp, "%d", &Demanda[i][j]);
      }
    }
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        fscanf(fp, "%d", &Asignacion[i][j]);
      }
    }
    fclose(fp);
    }
  } else {
    print_help();
    return 1;
  }

  // Calcular la matriz Necesidad
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      Necesidad[i][j] = Demanda[i][j] - Asignacion[i][j];
    }
  }
}
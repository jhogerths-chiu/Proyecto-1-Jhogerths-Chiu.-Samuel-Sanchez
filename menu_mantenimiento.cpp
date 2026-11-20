// CAPA DE PRESENTACION - Mantenimiento: integridad y respaldo.
#include "taller.h"
#include <iostream>
#include <cstdio>
using namespace std;

static void menuVerificarIntegridad() {
    static ReporteIntegridad r;     // static: la estructura es grande (~32 KB)
    CodigoError e = verificarIntegridad(r);
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    cout << "\n";
    for (int i = 0; i < r.numMensajes; i++) cout << r.mensajes[i] << "\n";
    if (r.problemas > r.numMensajes)
        cout << "... y " << (r.problemas - r.numMensajes) << " problemas mas (no se muestran).\n";

    int w[1] = {53};
    cout << "\n";
    lineaBorde(w, 1, '=');
    printf("| %-53s |\n", "REPORTE DE INTEGRIDAD DEL SISTEMA");
    lineaBorde(w, 1, '=');
    char b[100];
    snprintf(b, sizeof b, "Vehiculos verificados : %d", r.vehiculosVerificados);  printf("| %-53s |\n", b);
    snprintf(b, sizeof b, "Mecanicos verificados : %d", r.mecanicosVerificados);  printf("| %-53s |\n", b);
    snprintf(b, sizeof b, "Repuestos verificados : %d", r.repuestosVerificados);  printf("| %-53s |\n", b);
    snprintf(b, sizeof b, "Ordenes verificadas   : %d", r.ordenesVerificadas);    printf("| %-53s |\n", b);
    printf("| %-53s |\n", "");
    snprintf(b, sizeof b, "Problemas encontrados : %d", r.problemas);             printf("| %-53s |\n", b);
    snprintf(b, sizeof b, "Estado: %s", r.problemas == 0 ? "SISTEMA INTEGRO" : "SE ENCONTRARON PROBLEMAS");
    printf("| %-53s |\n", b);
    lineaBorde(w, 1, '=');
}

static void menuCrearBackup() {
    cout << "\n--- CREAR RESPALDO ---\n";
    if (!pedirConfirmacion("Copiar todos los archivos .bin a una carpeta de respaldo")) {
        cout << "Respaldo cancelado.\n";
        return;
    }
    char ruta[120];
    CodigoError e = crearBackup(ruta, sizeof ruta);
    if (e == E_OK) cout << "EXITO: Respaldo creado en la carpeta '" << ruta << "'.\n";
    else mostrarErrorSistema(e);
}

void menuMantenimiento() {
    const char* ops[2] = {"Verificar integridad", "Crear respaldo (backup)"};
    int op;
    do {
        imprimirMenu("MANTENIMIENTO", nullptr, ops, 2, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 2)) return;
        switch (op) {
            case 1: menuVerificarIntegridad(); break;
            case 2: menuCrearBackup(); break;
        }
    } while (op != 0);
}

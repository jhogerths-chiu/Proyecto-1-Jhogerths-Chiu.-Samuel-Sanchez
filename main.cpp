// Punto de entrada: arranque (archivos) y menu principal. La logica vive en los demas modulos.
#include <iostream>
#include <cstdio>
#include <cstring>
#include "taller.h"
using namespace std;

// Primer arranque: solicita los datos del taller y crea taller.bin
static bool crearTallerInicial(Taller& t) {
    cout << "\n=== PRIMER ARRANQUE: DATOS DEL TALLER ===\n";
    memset(&t, 0, sizeof t);
    if (!pedirTexto("Nombre del taller: ", t.nombre, sizeof t.nombre, nullptr, "") ||
        !pedirTexto("RIF: ", t.rif, sizeof t.rif, nullptr, "") ||
        !pedirTexto("Direccion: ", t.direccion, sizeof t.direccion, nullptr, "") ||
        !pedirTexto("Telefono: ", t.telefono, sizeof t.telefono, nullptr, ""))
        return false;
    CodigoError e = guardarTaller(t);
    if (e != E_OK) { mostrarErrorSistema(e); return false; }
    cout << "EXITO: Datos del taller guardados.\n";
    return true;
}

int main() {
    CodigoError e = inicializarSistema();
    if (e != E_OK) {
        mostrarErrorSistema(e);
        cout << "No se puede iniciar el sistema.\n";
        return 1;
    }

    Taller taller;
    if (!existeTaller()) {
        if (!crearTallerInicial(taller)) {
            cout << "Los datos del taller son obligatorios. Cerrando el sistema.\n";
            return 1;
        }
    } else {
        e = leerTaller(taller);
        if (e != E_OK) { mostrarErrorSistema(e); return 1; }
    }

    const char* ops[6] = {"Gestion de Vehiculos", "Gestion de Mecanicos",
                          "Inventario de Repuestos", "Ordenes de Servicio",
                          "Reportes", "Mantenimiento"};
    char subtitulo[120];
    snprintf(subtitulo, sizeof subtitulo, "Taller: %s", taller.nombre);

    int op = -1;
    while (true) {
        imprimirMenu("SISTEMA DE GESTION DE TALLER", subtitulo, ops, 6, "Salir");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 6)) break;   // fin de entrada / CANCELAR
        if (op == 0) {
            if (pedirConfirmacion("Salir del programa")) break;
            continue;
        }
        switch (op) {
            case 1: menuGestionVehiculos(); break;
            case 2: menuGestionMecanicos(); break;
            case 3: menuGestionInventario(); break;
            case 4: menuGestionOrdenes(); break;
            case 5: menuReportes(); break;
            case 6: menuMantenimiento(); break;
        }
    }

    cout << "\nCerrando el sistema... (los datos quedan guardados en la carpeta datos/)\n";
    return 0;
}

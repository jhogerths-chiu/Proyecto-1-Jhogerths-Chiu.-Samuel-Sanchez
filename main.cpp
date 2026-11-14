// Punto de entrada: solo menu principal. Toda la logica vive en los demas modulos.
#include <iostream>
#include <cstdio>
#include <cstring>
#include "taller.h"
using namespace std;

int main() {
    SistemaTaller sistema;
    Taller datosTaller;
    memset(&datosTaller, 0, sizeof datosTaller);
    copiarTexto(datosTaller.nombre, "Autoservicio El Lago", sizeof datosTaller.nombre);
    copiarTexto(datosTaller.rif, "J-12345678-9", sizeof datosTaller.rif);
    copiarTexto(datosTaller.direccion, "Avenida Principal, Maracaibo", sizeof datosTaller.direccion);
    copiarTexto(datosTaller.telefono, "0261-1234567", sizeof datosTaller.telefono);

    inicializarSistema(&sistema, datosTaller);

    const char* ops[4] = {"Gestion de Vehiculos", "Gestion de Mecanicos",
                          "Inventario de Repuestos", "Ordenes de Servicio"};
    char subtitulo[120];
    snprintf(subtitulo, sizeof subtitulo, "Taller: %s", sistema.taller.nombre);

    int op = -1;
    while (true) {
        imprimirMenu("SISTEMA DE GESTION DE TALLER", subtitulo, ops, 4, "Salir");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 4)) break;   // fin de entrada / CANCELAR
        if (op == 0) {
            if (pedirConfirmacion("Salir del programa (los datos en memoria se perderan)")) break;
            continue;
        }
        switch (op) {
            case 1: menuGestionVehiculos(&sistema); break;
            case 2: menuGestionMecanicos(&sistema); break;
            case 3: menuGestionInventario(&sistema); break;
            case 4: menuGestionOrdenes(&sistema); break;
        }
    }

    cout << "\nCerrando el sistema...\n";
    liberarSistema(&sistema);   // libera TODA la memoria dinamica, incluidos los detalles de cada orden
    return 0;
}

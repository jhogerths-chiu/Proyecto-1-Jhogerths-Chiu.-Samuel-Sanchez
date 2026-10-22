#include <iostream>
#include "taller.h"

using namespace std;

int main() {
    SistemaTaller sistema;
    Taller datos = {"Sam y Jho", "Maracaibo Edo. Zulia"};
    inicializarSistema(&sistema, datos);

    int opcion;
    do {
        cout << "╔══════════════════════════════════════╗\n";
        cout << "║      SISTEMA DE GESTION DE TALLER    ║\n";
        cout << "║               Sam y Jho              ║\n";
        cout << "╠══════════════════════════════════════╣\n";
        cout << "║  1. Gestion de Vehiculos             ║\n";
        cout << "║  2. Gestion de Mecanicos             ║\n";
        cout << "║  3. Inventario de Repuestos          ║\n";
        cout << "║  4. Ordenes de Servicio              ║\n";
        cout << "║  0. Salir                            ║\n";
        cout << "╚══════════════════════════════════════╝\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1:
                menuRegistrarVehiculo(&sistema);
                break;
            case 2:
                menuRegistrarMecanico(&sistema);
                break;
            case 3:
                menuRegistrarRepuesto(&sistema);
                break;
            case 4:
                menuGestionOrdenes(&sistema);
                break;
            case 0:
                cout << "\nGuardando y liberando memoria del sistema...\n";
                liberarSistema(&sistema);
                cout << "Saliendo del programa con exito!\n";
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo.\n";
        }
    } while(opcion != 0);

    return 0;
}
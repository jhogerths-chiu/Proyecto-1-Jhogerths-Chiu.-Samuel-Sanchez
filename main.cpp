#include <iostream>
#include "taller.h"

using namespace std;

// Declaraciones de las funciones definidas en vehiculos.cpp
void menuRegistrarVehiculo(SistemaTaller* s);
void menuListarVehiculos(SistemaTaller* s);

int main() {
    SistemaTaller sistema;
    Taller datosTaller = {"Autoservicio El Chino y el marico", "J-12345678-9", "Avenida Principal, Maracaibo", "0414-1234567"};
    
    // Inicializar la memoria del sistema
    inicializarSistema(&sistema, datosTaller);

    int opcion;
    do {
        cout << "\n========================================\n";
        cout << "   SISTEMA DE GESTION DE TALLER MECANICO\n";
        cout << "   Taller: " << sistema.taller.nombre << "\n";
        cout << "========================================\n";
        cout << "1. Registrar Vehiculo\n";
        cout << "2. Listar Vehiculos\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                menuRegistrarVehiculo(&sistema);
                break;
            case 2:
                menuListarVehiculos(&sistema);
                break;
            case 0:
                cout << "Saliendo del sistema...\n";
                break;
            default:
                cout << "Opcion invalida. Intente de nuevo.\n";
        }
    } while (opcion != 0);

    // Liberar toda la memoria reservada antes de salir
    liberarSistema(&sistema);
    return 0;
}
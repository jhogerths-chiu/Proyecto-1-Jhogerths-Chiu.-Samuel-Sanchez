#include "taller.h"
#include <iostream>

using namespace std;

int main() {
    SistemaTaller sistema;
    Taller infoTaller = {"Sam y Jho", "Maracaibo, Edo. Zulia"};
    
    // Inicializar el sistema y la memoria dinámica
    inicializarSistema(&sistema, infoTaller);

    int opcion = 0;
    do {
        cout << "\n========================================\n";
        cout << "    SISTEMA DE GESTION - " << sistema.taller.nombre << "\n";
        cout << "========================================\n";
        cout << "1. Registrar Vehiculo\n";
        cout << "2. Listar Vehiculos\n";
        cout << "3. Registrar Mecanico\n";
        cout << "4. Listar Mecanicos\n";
        cout << "5. Registrar Repuesto\n";
        cout << "6. Listar Repuestos (Inventario)\n";
        cout << "7. Salir\n";
        cout << "Seleccione una opcion: ";
        
        if (!(cin >> opcion)) {
            cout << "Entrada invalida. Intente de nuevo.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (opcion) {
            case 1:
                menuRegistrarVehiculo(&sistema);
                break;
            case 2:
                menuListarVehiculos(&sistema);
                break;
            case 3:
                menuRegistrarMecanico(&sistema);
                break;
            case 4:
                listarMecanicos(&sistema);
                break;
            case 5:
                menuRegistrarRepuesto(&sistema);
                break;
            case 6:
                listarRepuestos(&sistema);
                break;
            case 7:
                cout << "\nGuardando y liberando memoria del sistema...\n";
                liberarSistema(&sistema);
                cout << "Saliendo del programa con exito!\n";
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo.\n";
        }
    } while (opcion != 7);

    return 0;
}
#include <iostream>
#include <iomanip>
#include <cstring>
#include "taller.h"

using namespace std;

// Funcion auxiliar para agregar orden con redimensionamiento dinamico (2x)
OrdenServicio* agregarOrdenSistema(SistemaTaller* s, OrdenServicio o) {
    if (s->numOrdenes >= s->capacidadOrdenes) {
        s->capacidadOrdenes = (s->capacidadOrdenes == 0) ? 5 : s->capacidadOrdenes * 2;
        OrdenServicio* nuevoArreglo = new OrdenServicio[s->capacidadOrdenes];
        for (int i = 0; i < s->numOrdenes; i++) {
            nuevoArreglo[i] = s->ordenes[i];
        }
        delete[] s->ordenes;
        s->ordenes = nuevoArreglo;
    }
    o.id = s->siguienteIdOrden++;
    s->ordenes[s->numOrdenes] = o;
    s->numOrdenes++;
    return &s->ordenes[s->numOrdenes - 1];
}

OrdenServicio* buscarOrdenPorId(SistemaTaller* s, int id) {
    for (int i = 0; i < s->numOrdenes; i++) {
        if (s->ordenes[i].id == id) {
            return &s->ordenes[i];
        }
    }
    return nullptr;
}

// 6. Ver detalle de orden (Formato de salida 9.1 del documento)
void mostrarDetalleOrden(SistemaTaller* s, int idOrden) {
    OrdenServicio* ord = buscarOrdenPorId(s, idOrden);
    if (!ord) {
        cout << "\n[Error] Orden de servicio #" << idOrden << " no encontrada.\n";
        return;
    }

    Vehiculo* v = buscarVehiculoPorPlaca(s, ord->placaVehiculo);
    Mecanico* m = buscarMecanicoPorCedula(s, ord->cedulaMecanico);

    cout << "\n╔═════════════════════════════════════════════════════════════╗\n";
    cout << "║                     ORDEN DE SERVICIO #" << setfill(' ') << setw(2) << ord->id << "                   ║\n";
    cout << "╠═════════════════════════════════════════════════════════════╣\n";
    cout << "║ Estado   : " << left << setw(32) << ord->estado 
         << " Apertura: " << setw(10) << ord->fechaApertura << " ║\n";
    cout << "║                                             Cierre  : " << setw(10) << ord->fechaCierre << " ║\n";
    cout << "║ Vehiculo : " << setw(47) << (v ? (string(v->placa) + " - " + v->marca + " " + v->modelo + " (" + to_string(v->kilometraje) + " km)").c_str() : "N/A") << " ║\n";
    cout << "║ Dueno    : " << setw(47) << (v ? v->nombrePropietario : "N/A") << " ║\n";
    cout << "║ Mecanico : " << setw(47) << (m ? (string(m->nombre) + " (" + m->especialidad + ")").c_str() : "N/A") << " ║\n";
    cout << "║ Falla    : " << setw(47) << ord->descripcionFalla << " ║\n";
    cout << "╠══════════╦═════════════════════════════╦══════╦═══════╦═════╣\n";
    cout << "║ Codigo   ║ Repuesto                    ║ Cant ║ P.Unit║Total║\n";
    cout << "╠══════════╬═════════════════════════════╬══════╬═══════╬═════╣\n";

    float totalRepuestos = 0.0f;
    for (int i = 0; i < ord->numDetalles; i++) {
        float subtotal = ord->detalles[i].cantidad * ord->detalles[i].precioUnitario;
        totalRepuestos += subtotal;
        
        // Buscar descripcion del repuesto en el inventario
        Repuesto* rep = buscarRepuestoPorCodigo(s, ord->detalles[i].codigoRepuesto);
        const char* descRep = rep ? rep->descripcion : "Repuesto generico";

        cout << "║ " << left << setw(9) << ord->detalles[i].codigoRepuesto
             << "║ " << setw(28) << descRep
             << "║ " << right << setw(4) << ord->detalles[i].cantidad
             << "║ " << setw(6) << fixed << setprecision(2) << ord->detalles[i].precioUnitario
             << "║ " << setw(5) << subtotal << "║\n";
    }

    float totalGeneral = totalRepuestos + ord->manoDeObra;

    cout << "╠══════════╩═════════════════════════════╩══════╬═══════╩═════╣\n";
    cout << "║                                Repuestos     ║ " << right << setw(11) << fixed << setprecision(2) << totalRepuestos << " ║\n";
    cout << "║                                Mano de obra  ║ " << setw(11) << ord->manoDeObra << " ║\n";
    cout << "║                                TOTAL         ║ " << setw(11) << totalGeneral << " ║\n";
    cout << "╚═══════════════════════════════════════════════╩═════════════╝\n";
}

// 7. Listar ordenes
void listarOrdenes(const SistemaTaller* s) {
    if (s->numOrdenes == 0) {
        cout << "\nNo hay ordenes de servicio registradas en el sistema.\n";
        return;
    }
    cout << "\n=== LISTA DE ORDENES DE SERVICIO ===" << endl;
    for (int i = 0; i < s->numOrdenes; i++) {
        cout << "ID: #" << s->ordenes[i].id 
             << " | Placa: " << s->ordenes[i].placaVehiculo 
             << " | Estado: " << s->ordenes[i].estado 
             << " | Total: $" << s->ordenes[i].total << endl;
    }
}

// Submenu principal de Ordenes de Servicio (8 opciones segun documento)
void menuGestionOrdenes(SistemaTaller* s) {
    int opcion;
    do {
        cout << "\n╔══════════════════════════════════════╗\n";
        cout << "║          ORDENES DE SERVICIO         ║\n";
        cout << "╠══════════════════════════════════════╣\n";
        cout << "║  1. Abrir orden                      ║\n";
        cout << "║  2. Agregar repuesto a orden         ║\n";
        cout << "║  3. Quitar repuesto de orden         ║\n";
        cout << "║  4. Cerrar orden                     ║\n";
        cout << "║  5. Anular orden                     ║\n";
        cout << "║  6. Ver detalle de orden             ║\n";
        cout << "║  7. Listar ordenes                   ║\n";
        cout << "║  8. Volver al menu principal         ║\n";
        cout << "╚══════════════════════════════════════╝\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch(opcion) {
            case 1: { // 1. Abrir orden
                OrdenServicio nueva;
                cout << "Ingrese la placa del vehiculo: ";
                cin.getline(nueva.placaVehiculo, 20);
                
                // Validar que el vehiculo exista
                if (!buscarVehiculoPorPlaca(s, nueva.placaVehiculo)) {
                    cout << "[Error] El vehiculo con placa " << nueva.placaVehiculo << " no esta registrado.\n";
                    break;
                }

                cout << "Ingrese la cedula del mecanico: ";
                cin.getline(nueva.cedulaMecanico, 20);

                // Validar que el mecanico exista
                if (!buscarMecanicoPorCedula(s, nueva.cedulaMecanico)) {
                    cout << "[Error] El mecanico con cedula " << nueva.cedulaMecanico << " no esta registrado.\n";
                    break;
                }

                cout << "Ingrese la descripcion de la falla: ";
                cin.getline(nueva.descripcionFalla, 250);

                cout << "Ingrese la fecha de apertura (YYYY-MM-DD): ";
                cin.getline(nueva.fechaApertura, 15);

                strcpy(nueva.estado, "ABIERTA");
                strcpy(nueva.fechaCierre, "N/A");
                nueva.manoDeObra = 0.0f;
                nueva.detalles = nullptr;
                nueva.numDetalles = 0;
                nueva.capacidadDetalles = 0;
                nueva.total = 0.0f;

                agregarOrdenSistema(s, nueva);
                cout << "Orden de servicio abierta con exito!\n";
                break;
            }
            case 2: { // 2. Agregar repuesto a orden
                int id;
                cout << "Ingrese el ID de la orden: ";
                cin >> id;
                cin.ignore();
                
                OrdenServicio* ord = buscarOrdenPorId(s, id);
                if (!ord) {
                    cout << "[Error] Orden no encontrada.\n";
                    break;
                }
                if (strcmp(ord->estado, "ABIERTA") != 0) {
                    cout << "[Error] Solo se pueden modificar ordenes que esten ABIERTAS.\n";
                    break;
                }

                char cod[20];
                cout << "Ingrese el codigo del repuesto: ";
                cin.getline(cod, 20);

                Repuesto* rep = buscarRepuestoPorCodigo(s, cod);
                if (!rep) {
                    cout << "[Error] El repuesto no existe en el inventario.\n";
                    break;
                }

                int cant;
                cout << "Ingrese la cantidad a usar: ";
                cin >> cant;
                cin.ignore();

                if (cant > rep->stock) {
                    cout << "[Error] Stock insuficiente. Stock actual disponible: " << rep->stock << "\n";
                    break;
                }

                // Descontar del stock del inventario
                rep->stock -= cant;

                // Redimensionar detalles si es necesario (2x)
                if (ord->numDetalles >= ord->capacidadDetalles) {
                    ord->capacidadDetalles = (ord->capacidadDetalles == 0) ? 2 : ord->capacidadDetalles * 2;
                    DetalleOrden* nuevosDetalles = new DetalleOrden[ord->capacidadDetalles];
    
                    // Copiar anteriores
                    for (int i = 0; i < ord->numDetalles; i++) {
                        nuevosDetalles[i] = ord->detalles[i];
                    }
                    delete[] ord->detalles;
                    ord->detalles = nuevosDetalles;
                }

                strcpy(ord->detalles[ord->numDetalles].codigoRepuesto, cod);
                ord->detalles[ord->numDetalles].cantidad = cant;
                ord->detalles[ord->numDetalles].precioUnitario = rep->precioUnitario;
                ord->numDetalles++;

                cout << "Repuesto agregado exitosamente a la orden!\n";
                break;
            }
            case 3: // 3. Quitar repuesto de orden
                cout << "[Opcion 3: Quitar repuesto de orden - En desarrollo]\n";
                break;
            case 4: { // 4. Cerrar orden
                int id;
                cout << "Ingrese el ID de la orden a cerrar: ";
                cin >> id;
                cin.ignore();

                OrdenServicio* ord = buscarOrdenPorId(s, id);
                if (!ord) {
                    cout << "[Error] Orden no encontrada.\n";
                    break;
                }
                if (strcmp(ord->estado, "ABIERTA") != 0) {
                    cout << "[Error] La orden ya se encuentra cerrada o anulada.\n";
                    break;
                }

                cout << "Ingrese la fecha de cierre (YYYY-MM-DD): ";
                cin.getline(ord->fechaCierre, 15);

                cout << "Ingrese el costo de mano de obra: ";
                cin >> ord->manoDeObra;
                cin.ignore();

                float sumaRepuestos = 0.0f;
                for (int i = 0; i < ord->numDetalles; i++) {
                    sumaRepuestos += (ord->detalles[i].cantidad * ord->detalles[i].precioUnitario);
                }

                ord->total = sumaRepuestos + ord->manoDeObra;
                strcpy(ord->estado, "CERRADA");

                cout << "Orden cerrada con exito! Total calculado: $" << ord->total << "\n";
                break;
            }
            case 5: { // 5. Anular orden
                int id;
                cout << "Ingrese el ID de la orden a anular: ";
                cin >> id;
                cin.ignore();

                OrdenServicio* ord = buscarOrdenPorId(s, id);
                if (!ord) {
                    cout << "[Error] Orden no encontrada.\n";
                    break;
                }

                // Devolver repuestos al inventario
                for (int i = 0; i < ord->numDetalles; i++) {
                    Repuesto* rep = buscarRepuestoPorCodigo(s, ord->detalles[i].codigoRepuesto);
                    if (rep) {
                        rep->stock += ord->detalles[i].cantidad;
                    }
                }

                strcpy(ord->estado, "ANULADA");
                cout << "La orden ha sido anulada y los repuestos se devolvieron al stock!\n";
                break;
            }
            case 6: { // 6. Ver detalle de orden
                int id;
                cout << "Ingrese el ID de la orden a consultar: ";
                cin >> id;
                cin.ignore();
                mostrarDetalleOrden(s, id);
                break;
            }
            case 7: // 7. Listar ordenes
                listarOrdenes(s);
                break;
            case 8: // 8. Volver al menu principal
                cout << "Regresando al menu principal...\n";
                break;
            default:
                cout << "Opcion invalida. Intente de nuevo.\n";
        }
    } while(opcion != 8);
}
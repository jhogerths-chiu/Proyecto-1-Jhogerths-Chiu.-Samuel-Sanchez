#include "taller.h"
#include <iostream>
#include <cstring>

using namespace std;

// --- 1. GESTIÓN DE MEMORIA GLOBAL ---
void inicializarSistema(SistemaTaller* s, Taller datosTaller) {
    s->taller = datosTaller;

    s->capacidadVehiculos = 4;
    s->numVehiculos = 0;
    s->vehiculos = new Vehiculo[s->capacidadVehiculos];

    s->capacidadMecanicos = 4;
    s->numMecanicos = 0;
    s->mecanicos = new Mecanico[s->capacidadMecanicos];

    s->capacidadRepuestos = 4;
    s->numRepuestos = 0;
    s->repuestos = new Repuesto[s->capacidadRepuestos];

    s->capacidadOrdenes = 4;
    s->numOrdenes = 0;
    s->ordenes = new OrdenServicio[s->capacidadOrdenes];

    s->siguienteIdVehiculo = 1;
    s->siguienteIdMecanico = 1;
    s->siguienteIdRepuesto = 1;
    s->siguienteIdOrden = 1;
}

void liberarSistema(SistemaTaller* s) {
    for (int i = 0; i < s->numOrdenes; i++) {
        delete[] s->ordenes[i].detalles;
    }
    delete[] s->ordenes;
    delete[] s->repuestos;
    delete[] s->mecanicos;
    delete[] s->vehiculos;
}

// --- 2. LÓGICA DE VEHÍCULOS ---
Vehiculo* buscarVehiculoPorPlaca(SistemaTaller* s, const char* placa) {
    for (int i = 0; i < s->numVehiculos; i++) {
        if (strcmp(s->vehiculos[i].placa, placa) == 0) {
            return &(s->vehiculos[i]);
        }
    }
    return nullptr;
}

Vehiculo* agregarVehiculo(SistemaTaller* s, Vehiculo datos) {
    if (buscarVehiculoPorPlaca(s, datos.placa) != nullptr) return nullptr;
    if (datos.anio < 1950 || datos.anio > 2027 || datos.kilometraje < 0) return nullptr;

    // Redimensionamiento automático si se llena la capacidad (duplicar)
    if (s->numVehiculos >= s->capacidadVehiculos) {
        int nuevaCapacidad = s->capacidadVehiculos * 2;
        Vehiculo* nuevoArreglo = new Vehiculo[nuevaCapacidad];
        for (int i = 0; i < s->numVehiculos; i++) {
            nuevoArreglo[i] = s->vehiculos[i];
        }
        delete[] s->vehiculos;
        s->vehiculos = nuevoArreglo;
        s->capacidadVehiculos = nuevaCapacidad;
    }

    datos.id = s->siguienteIdVehiculo++;
    s->vehiculos[s->numVehiculos] = datos;
    s->numVehiculos++;

    return &(s->vehiculos[s->numVehiculos - 1]);
}

Vehiculo** listarVehiculos(SistemaTaller* s, int& cantidad) {
    cantidad = s->numVehiculos;
    if (cantidad == 0) return nullptr;

    Vehiculo** resultado = new Vehiculo*[cantidad];
    for (int i = 0; i < cantidad; i++) {
        resultado[i] = &(s->vehiculos[i]);
    }
    return resultado;
}

// --- 3. PRESENTACIÓN DE VEHÍCULOS ---
void mostrarVehiculo(Vehiculo* v) {
    if (v == nullptr) return;
    cout << "ID: " << v->id << " | Placa: " << v->placa 
         << " | " << v->marca << " " << v->modelo << " (" << v->anio << ") "
         << "| Propietario: " << v->nombrePropietario << "\n";
}
#include <limits> // Asegúrate de incluir esta librería arriba si no la tienes

void menuRegistrarVehiculo(SistemaTaller* s) {
    Vehiculo v;
    cout << "\n--- REGISTRAR VEHICULO ---\n";
    cout << "Placa: ";
    cin >> v.placa;
    cout << "Marca: ";
    cin >> v.marca;
    cout << "Modelo: ";
    cin >> v.modelo;
    
    cout << "Anio: ";
    while (!(cin >> v.anio)) {
        cout << "ERROR: Ingrese un año valido (numero): ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Kilometraje: ";
    while (!(cin >> v.kilometraje)) {
        cout << "ERROR: Ingrese un kilometraje valido (numero): ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Nombre del propietario: ";
    cin.ignore();
    cin.getline(v.nombrePropietario, 100);
    cout << "Cedula del propietario: ";
    cin >> v.cedulaPropietario;
    cout << "Telefono del propietario: ";
    cin >> v.telefonoPropietario;
    strcpy(v.fechaRegistro, "2026-10-16");

    char confirmar;
    cout << "Guardar registro? (S/N): ";
    cin >> confirmar;
    if (confirmar == 'S' || confirmar == 's') {
        if (agregarVehiculo(s, v) != nullptr) {
            cout << "EXITO: Vehiculo registrado.\n";
        } else {
            cout << "ERROR: Placa duplicada o datos invalidos.\n";
        }
    }
}

void menuListarVehiculos(SistemaTaller* s) {
    int cantidad = 0;
    Vehiculo** lista = listarVehiculos(s, cantidad);
    if (lista == nullptr || cantidad == 0) {
        cout << "No hay vehiculos registrados.\n";
        return;
    }
    cout << "\n--- LISTADO DE VEHICULOS ---\n";
    for (int i = 0; i < cantidad; i++) {
        mostrarVehiculo(lista[i]);
    }
    cout << "Total: " << cantidad << "\n";
    delete[] lista; // Liberar solo el arreglo temporal de punteros
}
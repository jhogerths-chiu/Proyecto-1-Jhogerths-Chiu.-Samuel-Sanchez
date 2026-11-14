// Inicializacion y liberacion de toda la memoria del sistema. SIN cin/cout.
#include "taller.h"

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
    // 1) Memoria anidada: el arreglo de detalles de CADA orden
    for (int i = 0; i < s->numOrdenes; i++) {
        delete[] s->ordenes[i].detalles;
        s->ordenes[i].detalles = nullptr;
    }
    // 2) Los arreglos principales
    delete[] s->ordenes;    s->ordenes = nullptr;
    delete[] s->repuestos;  s->repuestos = nullptr;
    delete[] s->mecanicos;  s->mecanicos = nullptr;
    delete[] s->vehiculos;  s->vehiculos = nullptr;
    s->numOrdenes = s->numRepuestos = s->numMecanicos = s->numVehiculos = 0;
    s->capacidadOrdenes = s->capacidadRepuestos = s->capacidadMecanicos = s->capacidadVehiculos = 0;
}

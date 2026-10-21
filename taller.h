#pragma once
#ifndef TALLER_H
#define TALLER_H

// --- ESTRUCTURAS BÁSICAS ---
struct Taller {
    char nombre[100];
    char direccion[150];
};

struct Vehiculo {
    int id;
    char placa[20];
    char marca[50];
    char modelo[50];
    int anio;
    int kilometraje;
    char nombrePropietario[100];
    char cedulaPropietario[20];
    char telefonoPropietario[20];
    char fechaRegistro[20];
};

struct Mecanico {
    int id;
    char cedula[20];
    char nombre[100];
    char especialidad[50]; 
    int ordenesActivas;
};

struct Repuesto {
    int id;
    char codigo[20];
    char descripcion[100];
    float precioUnitario;
    int stock;
};

struct DetalleOrden {
    char codigoRepuesto[20];
    int cantidad;
    float precioUnitario;
};

struct OrdenServicio {
    int id;
    char placaVehiculo[20];
    char cedulaMecanico[20];
    char descripcionFalla[250];
    DetalleOrden* detalles;
    int numDetalles;
    float total;
};

// --- CONTENEDOR GLOBAL COMPATIBLE ---
struct SistemaTaller {
    Taller taller;

    Vehiculo* vehiculos;
    int numVehiculos;
    int capacidadVehiculos;

    Mecanico* mecanicos;
    int numMecanicos;
    int capacidadMecanicos;

    Repuesto* repuestos;
    int numRepuestos;
    int capacidadRepuestos;

    OrdenServicio* ordenes;
    int numOrdenes;
    int capacidadOrdenes;

    int siguienteIdVehiculo;
    int siguienteIdMecanico;
    int siguienteIdRepuesto;
    int siguienteIdOrden;
};

// --- PROTOTIPOS GENERALES Y VEHÍCULOS ---
void inicializarSistema(SistemaTaller* s, Taller datosTaller);
void liberarSistema(SistemaTaller* s);
Vehiculo* buscarVehiculoPorPlaca(SistemaTaller* s, const char* placa);
Vehiculo* agregarVehiculo(SistemaTaller* s, Vehiculo datos);
Vehiculo** listarVehiculos(SistemaTaller* s, int& cantidad);
void mostrarVehiculo(Vehiculo* v);
void menuRegistrarVehiculo(SistemaTaller* s);
void menuListarVehiculos(SistemaTaller* s);

// --- PROTOTIPOS MECÁNICOS ---
Mecanico* buscarMecanicoPorCedula(SistemaTaller* s, const char* cedula);
Mecanico* agregarMecanico(SistemaTaller* s, Mecanico m);
void menuRegistrarMecanico(SistemaTaller* s);
void listarMecanicos(const SistemaTaller* s);

// --- PROTOTIPOS REPUESTOS ---
Repuesto* buscarRepuestoPorCodigo(SistemaTaller* s, const char* codigo);
Repuesto* agregarRepuesto(SistemaTaller* s, Repuesto r);
void menuRegistrarRepuesto(SistemaTaller* s);
void listarRepuestos(const SistemaTaller* s);

#endif
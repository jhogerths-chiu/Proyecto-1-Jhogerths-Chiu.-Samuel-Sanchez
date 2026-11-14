// CAPA DE PRESENTACION - Vehiculos. No modifica datos directamente: llama a la logica.
#include "taller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
using namespace std;

static const char* MSG_CEDULA =
    "ERROR: La cedula debe contener solo digitos (se admite prefijo V-, E- o J-).";
static const char* MSG_PLACA =
    "ERROR: La placa debe ser alfanumerica, de 6 a 7 caracteres.";

void mostrarVehiculo(Vehiculo* v) {
    if (v == nullptr) return;
    cout << "  Placa       : " << v->placa << "\n"
         << "  Vehiculo    : " << v->marca << " " << v->modelo << " " << v->anio << "\n"
         << "  Kilometraje : " << v->kilometraje << " km\n"
         << "  Propietario : " << v->nombrePropietario << " (C.I./RIF " << v->cedulaPropietario << ")\n"
         << "  Telefono    : " << v->telefonoPropietario << "\n"
         << "  Registrado  : " << v->fechaRegistro << "\n";
}

void mostrarListaVehiculos(Vehiculo** vehiculos, int cantidad) {
    int w[6] = {4, 8, 22, 4, 8, 18};
    lineaBorde(w, 6, '-');
    printf("| %-4s | %-8s | %-22s | %-4s | %-8s | %-18s |\n",
           "ID", "Placa", "Vehiculo", "Anio", "Km", "Propietario");
    lineaBorde(w, 6, '-');
    for (int i = 0; i < cantidad; i++) {
        Vehiculo* v = vehiculos[i];
        char desc[110];
        snprintf(desc, sizeof desc, "%s %s", v->marca, v->modelo);
        printf("| %-4d | %-8.8s | %-22.22s | %-4d | %-8d | %-18.18s |\n",
               v->id, v->placa, desc, v->anio, v->kilometraje, v->nombrePropietario);
    }
    lineaBorde(w, 6, '-');
    printf("Total de vehiculos: %d\n", cantidad);
}

// Pide los campos editables (comunes a registrar y actualizar). false = cancelado
static bool pedirDatosEditables(Vehiculo* v) {
    return pedirTexto("Marca: ", v->marca, sizeof v->marca, nullptr, "") &&
           pedirTexto("Modelo: ", v->modelo, sizeof v->modelo, nullptr, "") &&
           pedirEntero("Anio (1950-2027): ", &v->anio, 1950, 2027) &&
           pedirTexto("Nombre del propietario: ", v->nombrePropietario, sizeof v->nombrePropietario, nullptr, "") &&
           pedirTexto("Cedula/RIF del propietario: ", v->cedulaPropietario, sizeof v->cedulaPropietario, cedulaValida, MSG_CEDULA) &&
           pedirTexto("Telefono del propietario: ", v->telefonoPropietario, sizeof v->telefonoPropietario, nullptr, "");
}

void menuRegistrarVehiculo(SistemaTaller* s) {
    Vehiculo v;
    memset(&v, 0, sizeof v);
    cout << "\n--- REGISTRAR VEHICULO (escriba CANCELAR para abortar) ---\n";

    if (!pedirTexto("Placa: ", v.placa, sizeof v.placa, placaValida, MSG_PLACA)) {
        cout << "Registro cancelado. No se creo ningun vehiculo.\n"; return;
    }
    aMayusculas(v.placa);
    if (buscarVehiculoPorPlaca(s, v.placa) != nullptr) {
        cout << "ERROR: La placa '" << v.placa << "' ya esta registrada.\n";
        return;
    }
    if (!pedirTexto("Marca: ", v.marca, sizeof v.marca, nullptr, "") ||
        !pedirTexto("Modelo: ", v.modelo, sizeof v.modelo, nullptr, "") ||
        !pedirEntero("Anio (1950-2027): ", &v.anio, 1950, 2027) ||
        !pedirEntero("Kilometraje actual: ", &v.kilometraje, 0, INT_MAX) ||
        !pedirTexto("Nombre del propietario: ", v.nombrePropietario, sizeof v.nombrePropietario, nullptr, "") ||
        !pedirTexto("Cedula/RIF del propietario: ", v.cedulaPropietario, sizeof v.cedulaPropietario, cedulaValida, MSG_CEDULA) ||
        !pedirTexto("Telefono del propietario: ", v.telefonoPropietario, sizeof v.telefonoPropietario, nullptr, "")) {
        cout << "Registro cancelado. No se creo ningun vehiculo.\n";
        return;
    }

    if (!pedirConfirmacion("Guardar el vehiculo")) {
        cout << "Registro cancelado. No se creo ningun vehiculo.\n";
        return;
    }
    Vehiculo* creado = agregarVehiculo(s, v);
    if (creado == nullptr) cout << "ERROR: No se pudo registrar el vehiculo '" << v.placa << "' (datos invalidos).\n";
    else cout << "EXITO: Vehiculo '" << creado->placa << "' registrado a nombre de "
              << creado->nombrePropietario << ".\n";
}

void menuBuscarVehiculo(SistemaTaller* s) {
    const char* ops[2] = {"Buscar por placa exacta", "Buscar por propietario (parcial)"};
    imprimirMenu("BUSCAR VEHICULO", nullptr, ops, 2, "Volver");
    int op;
    if (!pedirEntero("Seleccione una opcion: ", &op, 0, 2) || op == 0) return;

    char texto[100];
    if (op == 1) {
        if (!pedirTexto("Placa: ", texto, sizeof texto, nullptr, "")) return;
        Vehiculo* v = buscarVehiculoPorPlaca(s, texto);
        if (v == nullptr) cout << "ERROR: No existe un vehiculo con la placa '" << texto << "'.\n";
        else { cout << "\nVehiculo encontrado:\n"; mostrarVehiculo(v); }
    } else {
        if (!pedirTexto("Nombre (o parte) del propietario: ", texto, sizeof texto, nullptr, "")) return;
        int cantidad = 0;
        Vehiculo** res = buscarVehiculosPorPropietario(s, texto, &cantidad);
        if (cantidad == 0) cout << "No se encontraron vehiculos cuyo propietario contenga '" << texto << "'.\n";
        else { cout << "\n"; mostrarListaVehiculos(res, cantidad); }
        delete[] res;   // solo el arreglo temporal de punteros
    }
}

void menuActualizarVehiculo(SistemaTaller* s) {
    cout << "\n--- ACTUALIZAR VEHICULO (escriba CANCELAR para abortar) ---\n";
    char placa[100];
    if (!pedirTexto("Placa del vehiculo a actualizar: ", placa, sizeof placa, nullptr, "")) return;
    Vehiculo* v = buscarVehiculoPorPlaca(s, placa);
    if (v == nullptr) { cout << "ERROR: No existe un vehiculo con la placa '" << placa << "'.\n"; return; }

    cout << "Datos actuales:\n";
    mostrarVehiculo(v);
    cout << "(la placa y el kilometraje no se pueden cambiar aqui)\n";

    Vehiculo nuevos = *v;
    if (!pedirDatosEditables(&nuevos)) { cout << "Actualizacion cancelada.\n"; return; }
    if (!pedirConfirmacion("Guardar los cambios")) { cout << "Actualizacion cancelada.\n"; return; }

    if (actualizarVehiculo(s, v->id, nuevos)) cout << "EXITO: Vehiculo '" << v->placa << "' actualizado.\n";
    else cout << "ERROR: No se pudo actualizar el vehiculo '" << v->placa << "' (datos invalidos).\n";
}

void menuEliminarVehiculo(SistemaTaller* s) {
    cout << "\n--- ELIMINAR VEHICULO ---\n";
    char placa[100];
    if (!pedirTexto("Placa del vehiculo a eliminar: ", placa, sizeof placa, nullptr, "")) return;
    Vehiculo* v = buscarVehiculoPorPlaca(s, placa);
    if (v == nullptr) { cout << "ERROR: No existe un vehiculo con la placa '" << placa << "'.\n"; return; }

    int n = contarOrdenesDeVehiculo(s, v->id);
    if (n > 0) {
        cout << "ERROR: El vehiculo '" << v->placa << "' tiene " << n
             << " ordenes registradas y no puede eliminarse.\n";
        return;
    }
    char copiaPlaca[10];
    copiarTexto(copiaPlaca, v->placa, sizeof copiaPlaca);
    if (!pedirConfirmacion("Eliminar definitivamente este vehiculo")) { cout << "Eliminacion cancelada.\n"; return; }
    if (eliminarVehiculo(s, v->id)) cout << "EXITO: Vehiculo '" << copiaPlaca << "' eliminado.\n";
    else cout << "ERROR: No se pudo eliminar el vehiculo '" << copiaPlaca << "'.\n";
}

void menuListarVehiculos(SistemaTaller* s) {
    int cantidad = 0;
    Vehiculo** lista = listarVehiculos(s, &cantidad);
    if (cantidad == 0) { cout << "\nNo hay vehiculos registrados.\n"; return; }
    cout << "\n";
    mostrarListaVehiculos(lista, cantidad);
    delete[] lista;   // se libera solo el arreglo temporal; los vehiculos siguen en el sistema
}

void menuGestionVehiculos(SistemaTaller* s) {
    const char* ops[5] = {"Registrar vehiculo", "Buscar vehiculo", "Actualizar vehiculo",
                          "Eliminar vehiculo", "Listar vehiculos"};
    int op;
    do {
        imprimirMenu("GESTION DE VEHICULOS", nullptr, ops, 5, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 5)) return;
        switch (op) {
            case 1: menuRegistrarVehiculo(s); break;
            case 2: menuBuscarVehiculo(s); break;
            case 3: menuActualizarVehiculo(s); break;
            case 4: menuEliminarVehiculo(s); break;
            case 5: menuListarVehiculos(s); break;
        }
    } while (op != 0);
}

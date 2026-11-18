// CAPA DE PRESENTACION - Vehiculos. No abre archivos: todo pasa por la logica.
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

void mostrarVehiculo(const Vehiculo& v) {
    char fecha[11];
    formatearFecha(v.fechaCreacion, fecha);
    cout << "  Placa       : " << v.placa << "\n"
         << "  Vehiculo    : " << v.marca << " " << v.modelo << " " << v.anio << "\n"
         << "  Kilometraje : " << v.kilometraje << " km\n"
         << "  Propietario : " << v.nombrePropietario << " (C.I./RIF " << v.cedulaPropietario << ")\n"
         << "  Telefono    : " << v.telefonoPropietario << "\n"
         << "  Registrado  : " << fecha << "\n";
}

void mostrarListaVehiculos(const Vehiculo* vehiculos, int cantidad, bool hayMas) {
    int w[6] = {4, 8, 22, 4, 8, 18};
    lineaBorde(w, 6, '-');
    printf("| %-4s | %-8s | %-22s | %-4s | %-8s | %-18s |\n",
           "ID", "Placa", "Vehiculo", "Anio", "Km", "Propietario");
    lineaBorde(w, 6, '-');
    for (int i = 0; i < cantidad; i++) {
        const Vehiculo& v = vehiculos[i];
        char desc[110];
        snprintf(desc, sizeof desc, "%s %s", v.marca, v.modelo);
        printf("| %-4d | %-8.8s | %-22.22s | %-4d | %-8d | %-18.18s |\n",
               v.id, v.placa, desc, v.anio, v.kilometraje, v.nombrePropietario);
    }
    lineaBorde(w, 6, '-');
    printf("Vehiculos mostrados: %d\n", cantidad);
    if (hayMas)
        printf("AVISO: Hay mas resultados. Se muestran solo los primeros %d; refine la busqueda.\n", MAX_RESULTADOS);
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

void menuRegistrarVehiculo() {
    Vehiculo v;
    memset(&v, 0, sizeof v);
    cout << "\n--- REGISTRAR VEHICULO (escriba CANCELAR para abortar) ---\n";

    if (!pedirTexto("Placa: ", v.placa, sizeof v.placa, placaValida, MSG_PLACA)) {
        cout << "Registro cancelado. No se creo ningun vehiculo.\n"; return;
    }
    aMayusculas(v.placa);
    Vehiculo existente;
    CodigoError e = obtenerVehiculoPorPlaca(v.placa, existente);
    if (e == E_OK) { cout << "ERROR: La placa '" << v.placa << "' ya esta registrada.\n"; return; }
    if (e != E_VEHICULO_NO_EXISTE) { mostrarErrorSistema(e); return; }

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

    int idNuevo = 0;
    e = registrarVehiculo(v, &idNuevo);
    if (e == E_OK)
        cout << "EXITO: Vehiculo '" << v.placa << "' registrado a nombre de " << v.nombrePropietario
             << " (ID " << idNuevo << ").\n";
    else if (e == E_DUPLICADO)
        cout << "ERROR: La placa '" << v.placa << "' ya esta registrada.\n";
    else if (e == E_DATOS_INVALIDOS)
        cout << "ERROR: No se pudo registrar el vehiculo '" << v.placa << "' (datos invalidos).\n";
    else
        mostrarErrorSistema(e);
}

void menuBuscarVehiculo() {
    const char* ops[3] = {"Buscar por placa exacta", "Buscar por propietario (parcial)",
                          "Buscar por ID (acceso directo)"};
    imprimirMenu("BUSCAR VEHICULO", nullptr, ops, 3, "Volver");
    int op;
    if (!pedirEntero("Seleccione una opcion: ", &op, 0, 3) || op == 0) return;

    if (op == 1) {
        char placa[100];
        if (!pedirTexto("Placa: ", placa, sizeof placa, nullptr, "")) return;
        Vehiculo v;
        CodigoError e = obtenerVehiculoPorPlaca(placa, v);
        if (e == E_OK) { cout << "\nVehiculo encontrado:\n"; mostrarVehiculo(v); }
        else if (e == E_VEHICULO_NO_EXISTE) cout << "ERROR: No existe un vehiculo activo con la placa '" << placa << "'.\n";
        else mostrarErrorSistema(e);
    } else if (op == 2) {
        char texto[100];
        if (!pedirTexto("Nombre (o parte) del propietario: ", texto, sizeof texto, nullptr, "")) return;
        Vehiculo res[MAX_RESULTADOS];
        int cantidad = 0;
        bool hayMas = false;
        CodigoError e = buscarVehiculosPorPropietario(texto, res, cantidad, hayMas);
        if (e != E_OK) mostrarErrorSistema(e);
        else if (cantidad == 0) cout << "No se encontraron vehiculos cuyo propietario contenga '" << texto << "'.\n";
        else { cout << "\n"; mostrarListaVehiculos(res, cantidad, hayMas); }
    } else {
        int id;
        if (!pedirEntero("ID del vehiculo: ", &id, 1, INT_MAX)) return;
        Vehiculo v;
        long pos = -1;
        CodigoError e = obtenerVehiculoPorID(id, v, &pos);
        if (e == E_OK || e == E_ELIMINADO)
            printf("[DEPURACION] Registro leido en la posicion %ld bytes (header %d + indice %d x %d bytes)\n",
                   pos, (int)sizeof(ArchivoHeader), id - 1, (int)sizeof(Vehiculo));
        if (e == E_OK) { cout << "\nVehiculo encontrado:\n"; mostrarVehiculo(v); }
        else if (e == E_ELIMINADO)
            cout << "El vehiculo con ID " << id << " (placa " << v.placa << ") fue ELIMINADO.\n";
        else if (e == E_ID_NO_EXISTE) cout << "ERROR: No existe un vehiculo con ID " << id << ".\n";
        else mostrarErrorSistema(e);
    }
}

void menuActualizarVehiculo() {
    cout << "\n--- ACTUALIZAR VEHICULO (escriba CANCELAR para abortar) ---\n";
    char placa[100];
    if (!pedirTexto("Placa del vehiculo a actualizar: ", placa, sizeof placa, nullptr, "")) return;
    Vehiculo v;
    CodigoError e = obtenerVehiculoPorPlaca(placa, v);
    if (e == E_VEHICULO_NO_EXISTE) { cout << "ERROR: No existe un vehiculo activo con la placa '" << placa << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    cout << "Datos actuales:\n";
    mostrarVehiculo(v);
    cout << "(la placa y el kilometraje no se pueden cambiar aqui)\n";

    Vehiculo nuevos = v;
    if (!pedirDatosEditables(&nuevos)) { cout << "Actualizacion cancelada.\n"; return; }
    if (!pedirConfirmacion("Guardar los cambios")) { cout << "Actualizacion cancelada.\n"; return; }

    e = actualizarVehiculo(v.id, nuevos);
    if (e == E_OK) cout << "EXITO: Vehiculo '" << v.placa << "' actualizado.\n";
    else if (e == E_DATOS_INVALIDOS) cout << "ERROR: No se pudo actualizar el vehiculo '" << v.placa << "' (datos invalidos).\n";
    else mostrarErrorSistema(e);
}

void menuEliminarVehiculo() {
    cout << "\n--- ELIMINAR VEHICULO ---\n";
    char placa[100];
    if (!pedirTexto("Placa del vehiculo a eliminar: ", placa, sizeof placa, nullptr, "")) return;
    Vehiculo v;
    CodigoError e = obtenerVehiculoPorPlaca(placa, v);
    if (e == E_VEHICULO_NO_EXISTE) { cout << "ERROR: No existe un vehiculo activo con la placa '" << placa << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    if (!pedirConfirmacion("Eliminar este vehiculo (borrado logico)")) { cout << "Eliminacion cancelada.\n"; return; }
    e = eliminarVehiculo(v.id);
    if (e == E_OK) cout << "EXITO: Vehiculo '" << v.placa << "' eliminado.\n";
    else if (e == E_VEHICULO_CON_ORDENES) {
        int n = 0;
        contarOrdenesDeVehiculo(v.id, n);
        cout << "ERROR: El vehiculo '" << v.placa << "' tiene " << n
             << " ordenes registradas y no puede eliminarse.\n";
    } else mostrarErrorSistema(e);
}

void menuListarVehiculos() {
    Vehiculo res[MAX_RESULTADOS];
    int cantidad = 0;
    bool hayMas = false;
    CodigoError e = listarVehiculos(res, cantidad, hayMas);
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cantidad == 0) { cout << "\nNo hay vehiculos registrados.\n"; return; }
    cout << "\n";
    mostrarListaVehiculos(res, cantidad, hayMas);
}

void menuGestionVehiculos() {
    const char* ops[5] = {"Registrar vehiculo", "Buscar vehiculo", "Actualizar vehiculo",
                          "Eliminar vehiculo", "Listar vehiculos"};
    int op;
    do {
        imprimirMenu("GESTION DE VEHICULOS", nullptr, ops, 5, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 5)) return;
        switch (op) {
            case 1: menuRegistrarVehiculo(); break;
            case 2: menuBuscarVehiculo(); break;
            case 3: menuActualizarVehiculo(); break;
            case 4: menuEliminarVehiculo(); break;
            case 5: menuListarVehiculos(); break;
        }
    } while (op != 0);
}

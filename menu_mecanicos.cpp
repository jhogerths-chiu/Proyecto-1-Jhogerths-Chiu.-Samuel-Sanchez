// CAPA DE PRESENTACION - Mecanicos.
#include "taller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
using namespace std;

static const char* MSG_CEDULA =
    "ERROR: La cedula debe contener solo digitos (se admite prefijo V-, E- o J-).";
static const char* MSG_ESPECIALIDAD =
    "ERROR: Especialidad invalida. Use MOTOR, FRENOS, ELECTRICIDAD, SUSPENSION o GENERAL.";
static const char* MSG_FECHA =
    "ERROR: Fecha invalida. Use el formato YYYY-MM-DD con valores validos.";

void mostrarMecanico(SistemaTaller* s, Mecanico* m) {
    if (m == nullptr) return;
    cout << "  Nombre        : " << m->nombre << "\n"
         << "  Cedula        : " << m->cedula << "\n"
         << "  Especialidad  : " << m->especialidad << "\n"
         << "  Fecha ingreso : " << m->fechaIngreso << "\n"
         << "  Ordenes abiertas: " << contarOrdenesAbiertasMecanico(s, m->id) << "\n";
}

void mostrarListaMecanicos(SistemaTaller* s, Mecanico** mecanicos, int cantidad) {
    int w[6] = {4, 24, 14, 12, 10, 8};
    lineaBorde(w, 6, '-');
    printf("| %-4s | %-24s | %-14s | %-12s | %-10s | %-8s |\n",
           "ID", "Nombre", "Cedula", "Especialidad", "Ingreso", "Abiertas");
    lineaBorde(w, 6, '-');
    for (int i = 0; i < cantidad; i++) {
        Mecanico* m = mecanicos[i];
        printf("| %-4d | %-24.24s | %-14.14s | %-12.12s | %-10s | %-8d |\n",
               m->id, m->nombre, m->cedula, m->especialidad, m->fechaIngreso,
               contarOrdenesAbiertasMecanico(s, m->id));
    }
    lineaBorde(w, 6, '-');
    printf("Total de mecanicos: %d\n", cantidad);
}

void menuRegistrarMecanico(SistemaTaller* s) {
    Mecanico m;
    memset(&m, 0, sizeof m);
    cout << "\n--- REGISTRAR MECANICO (escriba CANCELAR para abortar) ---\n";
    if (!pedirTexto("Nombre completo: ", m.nombre, sizeof m.nombre, nullptr, "") ||
        !pedirTexto("Cedula: ", m.cedula, sizeof m.cedula, cedulaValida, MSG_CEDULA) ||
        !pedirTexto("Especialidad (MOTOR/FRENOS/ELECTRICIDAD/SUSPENSION/GENERAL): ",
                    m.especialidad, sizeof m.especialidad, especialidadValida, MSG_ESPECIALIDAD) ||
        !pedirTexto("Fecha de ingreso (YYYY-MM-DD): ", m.fechaIngreso, sizeof m.fechaIngreso, fechaValida, MSG_FECHA)) {
        cout << "Registro cancelado. No se creo ningun mecanico.\n";
        return;
    }
    if (!pedirConfirmacion("Guardar el mecanico")) {
        cout << "Registro cancelado. No se creo ningun mecanico.\n";
        return;
    }
    CodigoError err;
    Mecanico* creado = agregarMecanico(s, m, &err);
    if (creado != nullptr) {
        cout << "EXITO: Mecanico '" << creado->nombre << "' (" << creado->especialidad << ") registrado.\n";
    } else if (err == E_DUPLICADO) {
        cout << "ERROR: La cedula '" << m.cedula << "' ya esta registrada para otro mecanico.\n";
    } else {
        cout << "ERROR: No se pudo registrar al mecanico '" << m.nombre << "' (datos invalidos).\n";
    }
}

void menuBuscarMecanico(SistemaTaller* s) {
    cout << "\n--- BUSCAR MECANICO ---\n";
    int id;
    if (!pedirEntero("ID del mecanico: ", &id, 1, INT_MAX)) return;
    Mecanico* m = buscarMecanicoPorID(s, id);
    if (m == nullptr) cout << "ERROR: No existe un mecanico con ID " << id << ".\n";
    else { cout << "\nMecanico encontrado:\n"; mostrarMecanico(s, m); }
}

void menuListarMecanicos(SistemaTaller* s) {
    int cantidad = 0;
    Mecanico** lista = listarMecanicos(s, &cantidad);
    if (cantidad == 0) { cout << "\nNo hay mecanicos registrados.\n"; return; }
    cout << "\n";
    mostrarListaMecanicos(s, lista, cantidad);
    delete[] lista;
}

void menuGestionMecanicos(SistemaTaller* s) {
    const char* ops[3] = {"Registrar mecanico", "Buscar mecanico por ID", "Listar mecanicos"};
    int op;
    do {
        imprimirMenu("GESTION DE MECANICOS", nullptr, ops, 3, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 3)) return;
        switch (op) {
            case 1: menuRegistrarMecanico(s); break;
            case 2: menuBuscarMecanico(s); break;
            case 3: menuListarMecanicos(s); break;
        }
    } while (op != 0);
}

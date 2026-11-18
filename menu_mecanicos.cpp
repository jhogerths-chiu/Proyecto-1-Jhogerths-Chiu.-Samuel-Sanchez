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

void mostrarMecanico(const Mecanico& m) {
    int abiertas = 0;
    contarOrdenesAbiertasMecanico(m.id, abiertas);
    cout << "  Nombre        : " << m.nombre << (m.eliminado ? "  (DADO DE BAJA)" : "") << "\n"
         << "  Cedula        : " << m.cedula << "\n"
         << "  Especialidad  : " << m.especialidad << "\n"
         << "  Fecha ingreso : " << m.fechaIngreso << "\n"
         << "  Estado        : " << (m.eliminado ? "BAJA" : "ACTIVO") << "\n"
         << "  Ordenes abiertas: " << abiertas << "\n";
}

void mostrarListaMecanicos(const Mecanico* mecanicos, int cantidad, bool hayMas) {
    int w[7] = {4, 24, 14, 12, 10, 8, 6};
    lineaBorde(w, 7, '-');
    printf("| %-4s | %-24s | %-14s | %-12s | %-10s | %-8s | %-6s |\n",
           "ID", "Nombre", "Cedula", "Especialidad", "Ingreso", "Abiertas", "Estado");
    lineaBorde(w, 7, '-');
    for (int i = 0; i < cantidad; i++) {
        const Mecanico& m = mecanicos[i];
        int abiertas = 0;
        contarOrdenesAbiertasMecanico(m.id, abiertas);
        printf("| %-4d | %-24.24s | %-14.14s | %-12.12s | %-10s | %-8d | %-6s |\n",
               m.id, m.nombre, m.cedula, m.especialidad, m.fechaIngreso, abiertas,
               m.eliminado ? "BAJA" : "ACTIVO");
    }
    lineaBorde(w, 7, '-');
    printf("Mecanicos mostrados: %d\n", cantidad);
    if (hayMas)
        printf("AVISO: Hay mas resultados. Se muestran solo los primeros %d.\n", MAX_RESULTADOS);
}

void menuRegistrarMecanico() {
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
    int idNuevo = 0;
    CodigoError e = registrarMecanico(m, &idNuevo);
    if (e == E_OK)
        cout << "EXITO: Mecanico '" << m.nombre << "' (" << m.especialidad << ") registrado con ID " << idNuevo << ".\n";
    else if (e == E_DUPLICADO)
        cout << "ERROR: La cedula '" << m.cedula << "' ya esta registrada para otro mecanico.\n";
    else if (e == E_DATOS_INVALIDOS)
        cout << "ERROR: No se pudo registrar al mecanico '" << m.nombre << "' (datos invalidos).\n";
    else
        mostrarErrorSistema(e);
}

void menuBuscarMecanico() {
    cout << "\n--- BUSCAR MECANICO ---\n";
    int id;
    if (!pedirEntero("ID del mecanico: ", &id, 1, INT_MAX)) return;
    Mecanico m;
    CodigoError e = obtenerMecanicoPorID(id, m);
    if (e == E_OK || e == E_ELIMINADO) { cout << "\nMecanico encontrado:\n"; mostrarMecanico(m); }
    else if (e == E_ID_NO_EXISTE) cout << "ERROR: No existe un mecanico con ID " << id << ".\n";
    else mostrarErrorSistema(e);
}

void menuListarMecanicos() {
    Mecanico res[MAX_RESULTADOS];
    int cantidad = 0;
    bool hayMas = false;
    CodigoError e = listarMecanicos(res, cantidad, hayMas);
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cantidad == 0) { cout << "\nNo hay mecanicos registrados.\n"; return; }
    cout << "\n";
    mostrarListaMecanicos(res, cantidad, hayMas);
}

void menuDarDeBajaMecanico() {
    cout << "\n--- DAR DE BAJA MECANICO (escriba CANCELAR para abortar) ---\n";
    int id;
    if (!pedirEntero("ID del mecanico: ", &id, 1, INT_MAX)) return;
    Mecanico m;
    CodigoError e = obtenerMecanicoPorID(id, m);
    if (e == E_ID_NO_EXISTE) { cout << "ERROR: No existe un mecanico con ID " << id << ".\n"; return; }
    if (e == E_ELIMINADO) { cout << "El mecanico '" << m.nombre << "' ya esta dado de baja.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    if (!pedirConfirmacion("Dar de baja a este mecanico (su historial se conserva)")) {
        cout << "Baja cancelada.\n";
        return;
    }
    e = darDeBajaMecanico(id);
    if (e == E_OK) cout << "EXITO: Mecanico '" << m.nombre << "' dado de baja.\n";
    else if (e == E_MECANICO_CON_ORDENES_ABIERTAS) {
        int n = 0;
        contarOrdenesAbiertasMecanico(id, n);
        cout << "ERROR: El mecanico '" << m.nombre << "' tiene " << n
             << " ordenes ABIERTAS y no puede darse de baja.\n";
    } else mostrarErrorSistema(e);
}

void menuGestionMecanicos() {
    const char* ops[4] = {"Registrar mecanico", "Buscar mecanico por ID", "Listar mecanicos",
                          "Dar de baja mecanico"};
    int op;
    do {
        imprimirMenu("GESTION DE MECANICOS", nullptr, ops, 4, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 4)) return;
        switch (op) {
            case 1: menuRegistrarMecanico(); break;
            case 2: menuBuscarMecanico(); break;
            case 3: menuListarMecanicos(); break;
            case 4: menuDarDeBajaMecanico(); break;
        }
    } while (op != 0);
}

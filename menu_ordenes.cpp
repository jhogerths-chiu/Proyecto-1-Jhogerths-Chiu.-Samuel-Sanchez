// CAPA DE PRESENTACION - Ordenes de servicio.
#include "taller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
using namespace std;

// ---------- Traduccion de codigos de error a mensajes con datos concretos ----------
static void mostrarErrorOrden(SistemaTaller* s, CodigoError e, int idOrden,
                              int idVehiculo, int idMecanico, int idRepuesto, int valor) {
    Vehiculo* v = buscarVehiculoPorID(s, idVehiculo);
    Mecanico* m = buscarMecanicoPorID(s, idMecanico);
    Repuesto* r = buscarRepuestoPorID(s, idRepuesto);
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    switch (e) {
        case E_VEHICULO_NO_EXISTE: cout << "ERROR: El vehiculo indicado no existe.\n"; break;
        case E_MECANICO_NO_EXISTE: cout << "ERROR: El mecanico indicado no existe.\n"; break;
        case E_REPUESTO_NO_EXISTE: cout << "ERROR: El repuesto indicado no existe.\n"; break;
        case E_ORDEN_NO_EXISTE:    cout << "ERROR: La orden #" << idOrden << " no existe.\n"; break;
        case E_VEHICULO_CON_ORDEN_ABIERTA: {
            OrdenServicio* ab = ordenAbiertaDeVehiculo(s, idVehiculo);
            cout << "ERROR: El vehiculo '" << (v ? v->placa : "?") << "' ya tiene la orden #"
                 << (ab ? ab->id : 0) << " ABIERTA.\n";
            break;
        }
        case E_MECANICO_LLENO:
            cout << "ERROR: El mecanico '" << (m ? m->nombre : "?") << "' ya tiene 3 ordenes abiertas.\n";
            break;
        case E_KM_MENOR:
            cout << "ERROR: El kilometraje " << valor << " es menor al ultimo registrado del vehiculo '"
                 << (v ? v->placa : "?") << "' (" << (v ? v->kilometraje : 0) << " km).\n";
            break;
        case E_ORDEN_NO_ABIERTA:
            cout << "ERROR: La orden #" << idOrden << " esta " << (o ? o->estado : "?")
                 << " y no puede modificarse.\n";
            break;
        case E_ORDEN_NO_ANULABLE:
            cout << "ERROR: La orden #" << idOrden << " esta " << (o ? o->estado : "?")
                 << " y no puede anularse.\n";
            break;
        case E_CANTIDAD_INVALIDA: cout << "ERROR: La cantidad debe ser mayor que 0.\n"; break;
        case E_STOCK_INSUFICIENTE:
            cout << "ERROR: Stock insuficiente de '" << (r ? r->nombre : "?") << "'. Disponible: "
                 << (r ? r->stock : 0) << ", solicitado: " << valor << ".\n";
            break;
        case E_REPUESTO_NO_EN_ORDEN:
            cout << "ERROR: El repuesto '" << (r ? r->nombre : "?") << "' no esta en la orden #" << idOrden << ".\n";
            break;
        case E_ORDEN_SIN_CONTENIDO:
            cout << "ERROR: La orden #" << idOrden << " no tiene repuestos ni mano de obra; no puede cerrarse.\n";
            break;
        case E_MANO_OBRA_INVALIDA: cout << "ERROR: La mano de obra no puede ser negativa.\n"; break;
        case E_DATOS_INVALIDOS:    cout << "ERROR: Datos invalidos.\n"; break;
        default:                   cout << "ERROR: La operacion no pudo completarse.\n"; break;
    }
}

// ---------- Salida ----------
static void fila(const char* texto) { printf("| %-65.65s |\n", texto); }
static void filaMonto(const char* etiqueta, float monto) { printf("| %-50s %14.2f |\n", etiqueta, monto); }

void mostrarDetalleOrden(SistemaTaller* s, OrdenServicio* o) {
    if (o == nullptr) return;
    Vehiculo* v = buscarVehiculoPorID(s, o->idVehiculo);
    Mecanico* m = buscarMecanicoPorID(s, o->idMecanico);
    int uno[1] = {65};
    int tabla[5] = {8, 25, 4, 8, 8};
    char b[400];

    lineaBorde(uno, 1, '-');
    snprintf(b, sizeof b, "ORDEN DE SERVICIO #%d", o->id); fila(b);
    lineaBorde(uno, 1, '-');
    snprintf(b, sizeof b, "Estado   : %-10s Apertura: %s", o->estado, o->fechaApertura); fila(b);
    snprintf(b, sizeof b, "Cierre   : %s", o->fechaCierre[0] ? o->fechaCierre : "-"); fila(b);
    snprintf(b, sizeof b, "Vehiculo : %s - %s %s %d (%d km)", v ? v->placa : "?", v ? v->marca : "?",
             v ? v->modelo : "?", v ? v->anio : 0, o->kilometrajeIngreso); fila(b);
    snprintf(b, sizeof b, "Dueno    : %s", v ? v->nombrePropietario : "?"); fila(b);
    snprintf(b, sizeof b, "Mecanico : %s (%s)", m ? m->nombre : "?", m ? m->especialidad : "?"); fila(b);
    snprintf(b, sizeof b, "Falla    : %s", o->descripcionFalla); fila(b);
    lineaBorde(tabla, 5, '-');
    printf("| %-8s | %-25s | %-4s | %-8s | %-8s |\n", "Codigo", "Repuesto", "Cant", "P.Unit", "Total");
    lineaBorde(tabla, 5, '-');
    if (o->numDetalles == 0) {
        printf("| %-8s | %-25s | %-4s | %-8s | %-8s |\n", "", "(sin repuestos)", "", "", "");
    }
    for (int i = 0; i < o->numDetalles; i++) {
        DetalleOrden* d = &o->detalles[i];
        Repuesto* r = buscarRepuestoPorID(s, d->idRepuesto);
        printf("| %-8.8s | %-25.25s | %4d | %8.2f | %8.2f |\n",
               r ? r->codigo : "?", r ? r->nombre : "?", d->cantidad, d->precioUnitario,
               d->cantidad * d->precioUnitario);
    }
    lineaBorde(tabla, 5, '-');
    filaMonto("Repuestos", calcularSubtotalRepuestos(o));
    filaMonto("Mano de obra", o->manoObra);
    filaMonto("TOTAL", calcularTotalOrden(o));
    lineaBorde(uno, 1, '-');
}

void mostrarListaOrdenes(SistemaTaller* s, OrdenServicio** ordenes, int cantidad) {
    int w[6] = {5, 8, 20, 8, 10, 10};
    lineaBorde(w, 6, '-');
    printf("| %-5s | %-8s | %-20s | %-8s | %-10s | %-10s |\n",
           "Orden", "Placa", "Mecanico", "Estado", "Apertura", "Total");
    lineaBorde(w, 6, '-');
    for (int i = 0; i < cantidad; i++) {
        OrdenServicio* o = ordenes[i];
        Vehiculo* v = buscarVehiculoPorID(s, o->idVehiculo);
        Mecanico* m = buscarMecanicoPorID(s, o->idMecanico);
        printf("| #%-4d | %-8.8s | %-20.20s | %-8s | %-10s | %10.2f |\n",
               o->id, v ? v->placa : "?", m ? m->nombre : "?", o->estado, o->fechaApertura,
               calcularTotalOrden(o));
    }
    lineaBorde(w, 6, '-');
    printf("Total de ordenes: %d\n", cantidad);
}

static bool estadoOrdenValido(const char* t) {
    return igualesIgnorandoMayusculas(t, "ABIERTA") || igualesIgnorandoMayusculas(t, "CERRADA") ||
           igualesIgnorandoMayusculas(t, "ANULADA");
}

// ---------- Operaciones ----------
void menuAbrirOrden(SistemaTaller* s) {
    cout << "\n--- ABRIR ORDEN DE SERVICIO (escriba CANCELAR para abortar) ---\n";
    int cm = 0;
    Mecanico** mecs = listarMecanicos(s, &cm);
    if (cm == 0) { cout << "ERROR: No hay mecanicos registrados.\n"; return; }
    mostrarListaMecanicos(s, mecs, cm);
    delete[] mecs;

    char placa[100];
    if (!pedirTexto("Placa del vehiculo: ", placa, sizeof placa, nullptr, "")) return;
    Vehiculo* v = buscarVehiculoPorPlaca(s, placa);
    if (v == nullptr) { cout << "ERROR: No existe un vehiculo con la placa '" << placa << "'.\n"; return; }

    int idMec;
    if (!pedirEntero("ID del mecanico responsable: ", &idMec, 1, INT_MAX)) return;
    Mecanico* m = buscarMecanicoPorID(s, idMec);
    if (m == nullptr) { cout << "ERROR: No existe un mecanico con ID " << idMec << ".\n"; return; }

    int km;
    char falla[200];
    if (!pedirEntero("Kilometraje de ingreso: ", &km, 0, INT_MAX) ||
        !pedirTexto("Descripcion de la falla: ", falla, sizeof falla, nullptr, "")) {
        cout << "Apertura cancelada. No se creo ninguna orden.\n";
        return;
    }
    if (!pedirConfirmacion("Abrir la orden")) { cout << "Apertura cancelada. No se creo ninguna orden.\n"; return; }

    CodigoError err;
    OrdenServicio* o = abrirOrden(s, v->id, m->id, km, falla, &err);
    if (o != nullptr)
        cout << "EXITO: Orden #" << o->id << " abierta para el vehiculo '" << v->placa
             << "' con el mecanico '" << m->nombre << "'.\n";
    else
        mostrarErrorOrden(s, err, 0, v->id, m->id, 0, km);
}

void menuAgregarRepuestoOrden(SistemaTaller* s) {
    cout << "\n--- AGREGAR REPUESTO A ORDEN (escriba CANCELAR para abortar) ---\n";
    int idOrden;
    if (!pedirEntero("Numero de orden: ", &idOrden, 1, INT_MAX)) return;
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) { mostrarErrorOrden(s, E_ORDEN_NO_EXISTE, idOrden, 0, 0, 0, 0); return; }
    if (strcmp(o->estado, "ABIERTA") != 0) { mostrarErrorOrden(s, E_ORDEN_NO_ABIERTA, idOrden, 0, 0, 0, 0); return; }

    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto* r = buscarRepuestoPorCodigo(s, codigo);
    if (r == nullptr) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    int cant;
    if (!pedirEntero("Cantidad (> 0): ", &cant, 1, INT_MAX)) return;

    CodigoError err = agregarRepuestoAOrden(s, idOrden, r->id, cant);
    if (err == E_OK)
        cout << "EXITO: " << cant << " x '" << r->nombre << "' agregado(s) a la orden #" << idOrden
             << ". Stock restante: " << r->stock << ".\n";
    else
        mostrarErrorOrden(s, err, idOrden, 0, 0, r->id, cant);
}

void menuQuitarRepuestoOrden(SistemaTaller* s) {
    cout << "\n--- QUITAR REPUESTO DE ORDEN (escriba CANCELAR para abortar) ---\n";
    int idOrden;
    if (!pedirEntero("Numero de orden: ", &idOrden, 1, INT_MAX)) return;
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) { mostrarErrorOrden(s, E_ORDEN_NO_EXISTE, idOrden, 0, 0, 0, 0); return; }
    if (strcmp(o->estado, "ABIERTA") != 0) { mostrarErrorOrden(s, E_ORDEN_NO_ABIERTA, idOrden, 0, 0, 0, 0); return; }

    char codigo[20];
    if (!pedirTexto("Codigo del repuesto a quitar: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto* r = buscarRepuestoPorCodigo(s, codigo);
    if (r == nullptr) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }

    CodigoError err = quitarRepuestoDeOrden(s, idOrden, r->id);
    if (err == E_OK)
        cout << "EXITO: '" << r->nombre << "' quitado de la orden #" << idOrden
             << ". Stock actual: " << r->stock << ".\n";
    else
        mostrarErrorOrden(s, err, idOrden, 0, 0, r->id, 0);
}

void menuCerrarOrden(SistemaTaller* s) {
    cout << "\n--- CERRAR ORDEN (escriba CANCELAR para abortar) ---\n";
    int idOrden;
    if (!pedirEntero("Numero de orden: ", &idOrden, 1, INT_MAX)) return;
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) { mostrarErrorOrden(s, E_ORDEN_NO_EXISTE, idOrden, 0, 0, 0, 0); return; }
    if (strcmp(o->estado, "ABIERTA") != 0) { mostrarErrorOrden(s, E_ORDEN_NO_ABIERTA, idOrden, 0, 0, 0, 0); return; }

    float mo;
    if (!pedirDecimal("Mano de obra (>= 0): ", &mo, 0, false)) return;
    if (!pedirConfirmacion("Cerrar la orden (ya no podra modificarse)")) { cout << "Cierre cancelado.\n"; return; }

    CodigoError err = cerrarOrden(s, idOrden, mo);
    if (err == E_OK) printf("EXITO: Orden #%d cerrada. Total: %.2f\n", idOrden, calcularTotalOrden(o));
    else mostrarErrorOrden(s, err, idOrden, 0, 0, 0, 0);
}

void menuAnularOrden(SistemaTaller* s) {
    cout << "\n--- ANULAR ORDEN (escriba CANCELAR para abortar) ---\n";
    int idOrden;
    if (!pedirEntero("Numero de orden: ", &idOrden, 1, INT_MAX)) return;
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) { mostrarErrorOrden(s, E_ORDEN_NO_EXISTE, idOrden, 0, 0, 0, 0); return; }
    if (strcmp(o->estado, "ABIERTA") != 0 && strcmp(o->estado, "CERRADA") != 0) {
        mostrarErrorOrden(s, E_ORDEN_NO_ANULABLE, idOrden, 0, 0, 0, 0);
        return;
    }
    if (!pedirConfirmacion("Anular la orden (se devolveran los repuestos al inventario)")) {
        cout << "Anulacion cancelada.\n";
        return;
    }
    CodigoError err = anularOrden(s, idOrden);
    if (err == E_OK) cout << "EXITO: Orden #" << idOrden << " ANULADA. Repuestos devueltos al inventario.\n";
    else mostrarErrorOrden(s, err, idOrden, 0, 0, 0, 0);
}

void menuVerDetalleOrden(SistemaTaller* s) {
    cout << "\n--- DETALLE DE ORDEN ---\n";
    int idOrden;
    if (!pedirEntero("Numero de orden: ", &idOrden, 1, INT_MAX)) return;
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) { mostrarErrorOrden(s, E_ORDEN_NO_EXISTE, idOrden, 0, 0, 0, 0); return; }
    cout << "\n";
    mostrarDetalleOrden(s, o);
}

void menuListarOrdenes(SistemaTaller* s) {
    const char* ops[3] = {"Todas las ordenes", "Por estado", "Por vehiculo (historial)"};
    imprimirMenu("LISTAR ORDENES", nullptr, ops, 3, "Volver");
    int op;
    if (!pedirEntero("Seleccione una opcion: ", &op, 0, 3) || op == 0) return;

    int cantidad = 0;
    OrdenServicio** lista = nullptr;
    if (op == 1) {
        lista = listarOrdenes(s, &cantidad);
    } else if (op == 2) {
        char estado[20];
        if (!pedirTexto("Estado (ABIERTA/CERRADA/ANULADA): ", estado, sizeof estado, estadoOrdenValido,
                        "ERROR: Estado invalido. Use ABIERTA, CERRADA o ANULADA.")) return;
        lista = listarOrdenesPorEstado(s, estado, &cantidad);
    } else {
        char placa[100];
        if (!pedirTexto("Placa del vehiculo: ", placa, sizeof placa, nullptr, "")) return;
        Vehiculo* v = buscarVehiculoPorPlaca(s, placa);
        if (v == nullptr) { cout << "ERROR: No existe un vehiculo con la placa '" << placa << "'.\n"; return; }
        lista = listarOrdenesPorVehiculo(s, v->id, &cantidad);
    }
    if (cantidad == 0) { cout << "\nNo se encontraron ordenes.\n"; return; }
    cout << "\n";
    mostrarListaOrdenes(s, lista, cantidad);
    delete[] lista;
}

void menuGestionOrdenes(SistemaTaller* s) {
    const char* ops[7] = {"Abrir orden", "Agregar repuesto a orden", "Quitar repuesto de orden",
                          "Cerrar orden", "Anular orden", "Ver detalle de orden", "Listar ordenes"};
    int op;
    do {
        imprimirMenu("ORDENES DE SERVICIO", nullptr, ops, 7, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 7)) return;
        switch (op) {
            case 1: menuAbrirOrden(s); break;
            case 2: menuAgregarRepuestoOrden(s); break;
            case 3: menuQuitarRepuestoOrden(s); break;
            case 4: menuCerrarOrden(s); break;
            case 5: menuAnularOrden(s); break;
            case 6: menuVerDetalleOrden(s); break;
            case 7: menuListarOrdenes(s); break;
        }
    } while (op != 0);
}

// CAPA DE PRESENTACION - Ordenes de servicio. No abre archivos: todo pasa por la logica.
#include "taller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
using namespace std;

// ---------- Traduccion de codigos de error a mensajes con datos concretos ----------
static void mostrarErrorOrden(CodigoError e, int idOrden, int idVehiculo, int idMecanico,
                              int idRepuesto, int valor) {
    Vehiculo v;  memset(&v, 0, sizeof v);
    Mecanico m;  memset(&m, 0, sizeof m);
    Repuesto r;  memset(&r, 0, sizeof r);
    OrdenServicio o; memset(&o, 0, sizeof o);
    obtenerVehiculoPorID(idVehiculo, v);
    obtenerMecanicoPorID(idMecanico, m);
    obtenerRepuestoPorID(idRepuesto, r);
    obtenerOrdenPorID(idOrden, o);
    switch (e) {
        case E_VEHICULO_NO_EXISTE: cout << "ERROR: El vehiculo indicado no existe.\n"; break;
        case E_VEHICULO_INACTIVO:  cout << "ERROR: El vehiculo '" << v.placa << "' esta eliminado.\n"; break;
        case E_MECANICO_NO_EXISTE: cout << "ERROR: El mecanico indicado no existe.\n"; break;
        case E_MECANICO_INACTIVO:
            cout << "ERROR: El mecanico '" << m.nombre << "' esta dado de baja y no recibe ordenes nuevas.\n";
            break;
        case E_REPUESTO_NO_EXISTE: cout << "ERROR: El repuesto indicado no existe.\n"; break;
        case E_ORDEN_NO_EXISTE:    cout << "ERROR: La orden #" << idOrden << " no existe.\n"; break;
        case E_VEHICULO_CON_ORDEN_ABIERTA: {
            int ab = 0;
            ordenAbiertaDeVehiculo(idVehiculo, ab);
            cout << "ERROR: El vehiculo '" << v.placa << "' ya tiene la orden #" << ab << " ABIERTA.\n";
            break;
        }
        case E_MECANICO_LLENO:
            cout << "ERROR: El mecanico '" << m.nombre << "' ya tiene 3 ordenes abiertas.\n";
            break;
        case E_KM_MENOR:
            cout << "ERROR: El kilometraje " << valor << " es menor al ultimo registrado del vehiculo '"
                 << v.placa << "' (" << v.kilometraje << " km).\n";
            break;
        case E_ORDEN_NO_ABIERTA:
            cout << "ERROR: La orden #" << idOrden << " esta " << o.estado << " y no puede modificarse.\n";
            break;
        case E_ORDEN_NO_ANULABLE:
            cout << "ERROR: La orden #" << idOrden << " esta " << o.estado << " y no puede anularse.\n";
            break;
        case E_CANTIDAD_INVALIDA: cout << "ERROR: La cantidad debe ser mayor que 0.\n"; break;
        case E_STOCK_INSUFICIENTE:
            cout << "ERROR: Stock insuficiente de '" << r.nombre << "'. Disponible: "
                 << r.stock << ", solicitado: " << valor << ".\n";
            break;
        case E_REPUESTO_NO_EN_ORDEN:
            cout << "ERROR: El repuesto '" << r.nombre << "' no esta en la orden #" << idOrden << ".\n";
            break;
        case E_ORDEN_SIN_CONTENIDO:
            cout << "ERROR: La orden #" << idOrden << " no tiene repuestos ni mano de obra; no puede cerrarse.\n";
            break;
        case E_MANO_OBRA_INVALIDA: cout << "ERROR: La mano de obra no puede ser negativa.\n"; break;
        case E_LINEAS_LLENAS:
            cout << "ERROR: La orden #" << idOrden << " alcanzo el limite de " << MAX_LINEAS_ORDEN
                 << " lineas de detalle. El repuesto no se agrego.\n";
            break;
        case E_DATOS_INVALIDOS:    cout << "ERROR: Datos invalidos.\n"; break;
        default: mostrarErrorSistema(e); break;
    }
}

// ---------- Salida ----------
static void fila(const char* texto) { printf("| %-65.65s |\n", texto); }
static void filaMonto(const char* etiqueta, float monto) { printf("| %-50s %14.2f |\n", etiqueta, monto); }

void mostrarDetalleOrden(const OrdenServicio& o) {
    Vehiculo v;  memset(&v, 0, sizeof v);
    Mecanico m;  memset(&m, 0, sizeof m);
    bool hayV = obtenerVehiculoPorID(o.idVehiculo, v) == E_OK;      // E_ELIMINADO tambien rellena
    if (!hayV && v.placa[0] == '\0') copiarTexto(v.placa, "?", sizeof v.placa);
    CodigoError em = obtenerMecanicoPorID(o.idMecanico, m);         // E_ELIMINADO = dado de baja
    if (em != E_OK && em != E_ELIMINADO) copiarTexto(m.nombre, "?", sizeof m.nombre);

    int uno[1] = {65};
    int tabla[5] = {8, 25, 4, 8, 8};
    char b[400];

    lineaBorde(uno, 1, '-');
    snprintf(b, sizeof b, "ORDEN DE SERVICIO #%d", o.id); fila(b);
    lineaBorde(uno, 1, '-');
    snprintf(b, sizeof b, "Estado   : %-10s Apertura: %s", o.estado, o.fechaApertura); fila(b);
    snprintf(b, sizeof b, "Cierre   : %s", o.fechaCierre[0] ? o.fechaCierre : "-"); fila(b);
    snprintf(b, sizeof b, "Vehiculo : %s - %s %s %d (%d km)", v.placa, v.marca, v.modelo, v.anio,
             o.kilometrajeIngreso); fila(b);
    snprintf(b, sizeof b, "Dueno    : %s", v.nombrePropietario); fila(b);
    snprintf(b, sizeof b, "Mecanico : %s (%s)%s", m.nombre, m.especialidad, em == E_ELIMINADO ? " [BAJA]" : ""); fila(b);
    snprintf(b, sizeof b, "Falla    : %s", o.descripcionFalla); fila(b);
    lineaBorde(tabla, 5, '-');
    printf("| %-8s | %-25s | %-4s | %-8s | %-8s |\n", "Codigo", "Repuesto", "Cant", "P.Unit", "Total");
    lineaBorde(tabla, 5, '-');
    if (o.numLineas == 0)
        printf("| %-8s | %-25s | %-4s | %-8s | %-8s |\n", "", "(sin repuestos)", "", "", "");
    for (int i = 0; i < o.numLineas; i++) {
        const DetalleOrden& d = o.lineas[i];
        Repuesto r;
        memset(&r, 0, sizeof r);
        CodigoError er = obtenerRepuestoPorID(d.idRepuesto, r);
        bool ok = (er == E_OK || er == E_ELIMINADO);
        printf("| %-8.8s | %-25.25s | %4d | %8.2f | %8.2f |\n",
               ok ? r.codigo : "?", ok ? r.nombre : "?", d.cantidad, d.precioUnitario,
               d.cantidad * d.precioUnitario);
    }
    lineaBorde(tabla, 5, '-');
    filaMonto("Repuestos", calcularSubtotalRepuestos(o));
    filaMonto("Mano de obra", o.manoObra);
    filaMonto("TOTAL", calcularTotalOrden(o));
    lineaBorde(uno, 1, '-');
}

void mostrarListaOrdenes(const OrdenServicio* ordenes, int cantidad, bool hayMas) {
    int w[6] = {5, 8, 20, 8, 10, 10};
    lineaBorde(w, 6, '-');
    printf("| %-5s | %-8s | %-20s | %-8s | %-10s | %-10s |\n",
           "Orden", "Placa", "Mecanico", "Estado", "Apertura", "Total");
    lineaBorde(w, 6, '-');
    for (int i = 0; i < cantidad; i++) {
        const OrdenServicio& o = ordenes[i];
        Vehiculo v;  memset(&v, 0, sizeof v);
        Mecanico m;  memset(&m, 0, sizeof m);
        obtenerVehiculoPorID(o.idVehiculo, v);
        obtenerMecanicoPorID(o.idMecanico, m);
        printf("| #%-4d | %-8.8s | %-20.20s | %-8s | %-10s | %10.2f |\n",
               o.id, v.placa[0] ? v.placa : "?", m.nombre[0] ? m.nombre : "?", o.estado,
               o.fechaApertura, calcularTotalOrden(o));
    }
    lineaBorde(w, 6, '-');
    printf("Ordenes mostradas: %d\n", cantidad);
    if (hayMas)
        printf("AVISO: Hay mas resultados. Se muestran solo las primeras %d.\n", MAX_RESULTADOS);
}

static bool estadoOrdenValido(const char* t) {
    return igualesIgnorandoMayusculas(t, "ABIERTA") || igualesIgnorandoMayusculas(t, "CERRADA") ||
           igualesIgnorandoMayusculas(t, "ANULADA");
}

// ---------- Operaciones ----------
void menuAbrirOrden() {
    cout << "\n--- ABRIR ORDEN DE SERVICIO (escriba CANCELAR para abortar) ---\n";
    Mecanico mecs[MAX_RESULTADOS];
    int cm = 0;
    bool hayMas = false;
    CodigoError e = listarMecanicos(mecs, cm, hayMas);
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cm == 0) { cout << "ERROR: No hay mecanicos registrados.\n"; return; }
    mostrarListaMecanicos(mecs, cm, hayMas);

    char placa[100];
    if (!pedirTexto("Placa del vehiculo: ", placa, sizeof placa, nullptr, "")) return;
    Vehiculo v;
    e = obtenerVehiculoPorPlaca(placa, v);
    if (e == E_VEHICULO_NO_EXISTE) { cout << "ERROR: No existe un vehiculo activo con la placa '" << placa << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    int idMec;
    if (!pedirEntero("ID del mecanico responsable: ", &idMec, 1, INT_MAX)) return;
    Mecanico m;
    e = obtenerMecanicoPorID(idMec, m);
    if (e == E_ID_NO_EXISTE) { cout << "ERROR: No existe un mecanico con ID " << idMec << ".\n"; return; }
    if (e == E_ELIMINADO) { mostrarErrorOrden(E_MECANICO_INACTIVO, 0, v.id, idMec, 0, 0); return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    int km;
    char falla[200];
    if (!pedirEntero("Kilometraje de ingreso: ", &km, 0, INT_MAX) ||
        !pedirTexto("Descripcion de la falla: ", falla, sizeof falla, nullptr, "")) {
        cout << "Apertura cancelada. No se creo ninguna orden.\n";
        return;
    }
    if (!pedirConfirmacion("Abrir la orden")) { cout << "Apertura cancelada. No se creo ninguna orden.\n"; return; }

    int idNuevo = 0;
    e = abrirOrden(v.id, m.id, km, falla, &idNuevo);
    if (e == E_OK)
        cout << "EXITO: Orden #" << idNuevo << " abierta para el vehiculo '" << v.placa
             << "' con el mecanico '" << m.nombre << "'.\n";
    else
        mostrarErrorOrden(e, 0, v.id, m.id, 0, km);
}

// Pide el numero de orden y la carga. false = no se puede continuar (ya informo el motivo)
static bool pedirOrden(OrdenServicio& o, bool soloAbierta) {
    int idOrden;
    if (!pedirEntero("Numero de orden: ", &idOrden, 1, INT_MAX)) return false;
    CodigoError e = obtenerOrdenPorID(idOrden, o);
    if (e == E_ID_NO_EXISTE) { mostrarErrorOrden(E_ORDEN_NO_EXISTE, idOrden, 0, 0, 0, 0); return false; }
    if (e != E_OK) { mostrarErrorSistema(e); return false; }
    if (soloAbierta && strcmp(o.estado, "ABIERTA") != 0) {
        mostrarErrorOrden(E_ORDEN_NO_ABIERTA, idOrden, 0, 0, 0, 0);
        return false;
    }
    return true;
}

void menuAgregarRepuestoOrden() {
    cout << "\n--- AGREGAR REPUESTO A ORDEN (escriba CANCELAR para abortar) ---\n";
    OrdenServicio o;
    if (!pedirOrden(o, true)) return;

    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto r;
    CodigoError e = obtenerRepuestoPorCodigo(codigo, r);
    if (e == E_REPUESTO_NO_EXISTE) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    int cant;
    if (!pedirEntero("Cantidad (> 0): ", &cant, 1, INT_MAX)) return;

    e = agregarRepuestoAOrden(o.id, r.id, cant);
    if (e == E_OK) {
        obtenerRepuestoPorID(r.id, r);     // stock actualizado, leido del disco
        cout << "EXITO: " << cant << " x '" << r.nombre << "' agregado(s) a la orden #" << o.id
             << ". Stock restante: " << r.stock << ".\n";
    } else
        mostrarErrorOrden(e, o.id, 0, 0, r.id, cant);
}

void menuQuitarRepuestoOrden() {
    cout << "\n--- QUITAR REPUESTO DE ORDEN (escriba CANCELAR para abortar) ---\n";
    OrdenServicio o;
    if (!pedirOrden(o, true)) return;

    char codigo[20];
    if (!pedirTexto("Codigo del repuesto a quitar: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto r;
    CodigoError e = obtenerRepuestoPorCodigo(codigo, r);
    if (e == E_REPUESTO_NO_EXISTE) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    e = quitarRepuestoDeOrden(o.id, r.id);
    if (e == E_OK) {
        obtenerRepuestoPorID(r.id, r);
        cout << "EXITO: '" << r.nombre << "' quitado de la orden #" << o.id
             << ". Stock actual: " << r.stock << ".\n";
    } else
        mostrarErrorOrden(e, o.id, 0, 0, r.id, 0);
}

void menuCerrarOrden() {
    cout << "\n--- CERRAR ORDEN (escriba CANCELAR para abortar) ---\n";
    OrdenServicio o;
    if (!pedirOrden(o, true)) return;

    float mo;
    if (!pedirDecimal("Mano de obra (>= 0): ", &mo, 0, false)) return;
    if (!pedirConfirmacion("Cerrar la orden (ya no podra modificarse)")) { cout << "Cierre cancelado.\n"; return; }

    CodigoError e = cerrarOrden(o.id, mo);
    if (e == E_OK) {
        obtenerOrdenPorID(o.id, o);
        printf("EXITO: Orden #%d cerrada. Total: %.2f\n", o.id, calcularTotalOrden(o));
    } else
        mostrarErrorOrden(e, o.id, 0, 0, 0, 0);
}

void menuAnularOrden() {
    cout << "\n--- ANULAR ORDEN (escriba CANCELAR para abortar) ---\n";
    OrdenServicio o;
    if (!pedirOrden(o, false)) return;
    if (strcmp(o.estado, "ABIERTA") != 0 && strcmp(o.estado, "CERRADA") != 0) {
        mostrarErrorOrden(E_ORDEN_NO_ANULABLE, o.id, 0, 0, 0, 0);
        return;
    }
    if (!pedirConfirmacion("Anular la orden (se devolveran los repuestos al inventario)")) {
        cout << "Anulacion cancelada.\n";
        return;
    }
    CodigoError e = anularOrden(o.id);
    if (e == E_OK) cout << "EXITO: Orden #" << o.id << " ANULADA. Repuestos devueltos al inventario.\n";
    else mostrarErrorOrden(e, o.id, 0, 0, 0, 0);
}

void menuVerDetalleOrden() {
    cout << "\n--- DETALLE DE ORDEN ---\n";
    OrdenServicio o;
    if (!pedirOrden(o, false)) return;
    cout << "\n";
    mostrarDetalleOrden(o);
}

void menuListarOrdenes() {
    const char* ops[3] = {"Todas las ordenes", "Por estado", "Por vehiculo (historial)"};
    imprimirMenu("LISTAR ORDENES", nullptr, ops, 3, "Volver");
    int op;
    if (!pedirEntero("Seleccione una opcion: ", &op, 0, 3) || op == 0) return;

    OrdenServicio res[MAX_RESULTADOS];
    int cantidad = 0;
    bool hayMas = false;
    CodigoError e;
    if (op == 1) {
        e = listarOrdenes(res, cantidad, hayMas);
    } else if (op == 2) {
        char estado[20];
        if (!pedirTexto("Estado (ABIERTA/CERRADA/ANULADA): ", estado, sizeof estado, estadoOrdenValido,
                        "ERROR: Estado invalido. Use ABIERTA, CERRADA o ANULADA.")) return;
        e = listarOrdenesPorEstado(estado, res, cantidad, hayMas);
    } else {
        char placa[100];
        if (!pedirTexto("Placa del vehiculo: ", placa, sizeof placa, nullptr, "")) return;
        Vehiculo v;
        e = obtenerVehiculoPorPlaca(placa, v);
        if (e == E_VEHICULO_NO_EXISTE) { cout << "ERROR: No existe un vehiculo activo con la placa '" << placa << "'.\n"; return; }
        if (e != E_OK) { mostrarErrorSistema(e); return; }
        e = listarOrdenesPorVehiculo(v.id, res, cantidad, hayMas);
    }
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cantidad == 0) { cout << "\nNo se encontraron ordenes.\n"; return; }
    cout << "\n";
    mostrarListaOrdenes(res, cantidad, hayMas);
}

void menuGestionOrdenes() {
    const char* ops[7] = {"Abrir orden", "Agregar repuesto a orden", "Quitar repuesto de orden",
                          "Cerrar orden", "Anular orden", "Ver detalle de orden", "Listar ordenes"};
    int op;
    do {
        imprimirMenu("ORDENES DE SERVICIO", nullptr, ops, 7, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 7)) return;
        switch (op) {
            case 1: menuAbrirOrden(); break;
            case 2: menuAgregarRepuestoOrden(); break;
            case 3: menuQuitarRepuestoOrden(); break;
            case 4: menuCerrarOrden(); break;
            case 5: menuAnularOrden(); break;
            case 6: menuVerDetalleOrden(); break;
            case 7: menuListarOrdenes(); break;
        }
    } while (op != 0);
}

// CAPA DE PRESENTACION - Inventario de repuestos.
#include "taller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
using namespace std;

void mostrarRepuesto(const Repuesto& r) {
    char fecha[11];
    formatearFecha(r.fechaCreacion, fecha);
    cout << "  Codigo       : " << r.codigo << "\n"
         << "  Nombre       : " << r.nombre << "\n"
         << "  Marca        : " << r.marca << "\n";
    printf("  Precio       : %.2f\n", r.precio);
    cout << "  Stock        : " << r.stock << " (minimo " << r.stockMinimo << ")"
         << (estaBajoStock(r) ? "  *** BAJO STOCK ***" : "") << "\n"
         << "  Registrado   : " << fecha << "\n";
}

void mostrarListaRepuestos(const Repuesto* repuestos, int cantidad, bool hayMas) {
    int w[8] = {4, 10, 28, 12, 9, 6, 6, 10};
    lineaBorde(w, 8, '-');
    printf("| %-4s | %-10s | %-28s | %-12s | %-9s | %-6s | %-6s | %-10s |\n",
           "ID", "Codigo", "Repuesto", "Marca", "Precio", "Stock", "Minimo", "Estado");
    lineaBorde(w, 8, '-');
    for (int i = 0; i < cantidad; i++) {
        const Repuesto& r = repuestos[i];
        printf("| %-4d | %-10.10s | %-28.28s | %-12.12s | %9.2f | %-6d | %-6d | %-10s |\n",
               r.id, r.codigo, r.nombre, r.marca, r.precio, r.stock, r.stockMinimo,
               estaBajoStock(r) ? "BAJO STOCK" : "");
    }
    lineaBorde(w, 8, '-');
    printf("Repuestos mostrados: %d\n", cantidad);
    if (hayMas)
        printf("AVISO: Hay mas resultados. Se muestran solo los primeros %d; refine la busqueda.\n", MAX_RESULTADOS);
}

void menuRegistrarRepuesto() {
    Repuesto r;
    memset(&r, 0, sizeof r);
    cout << "\n--- REGISTRAR REPUESTO (escriba CANCELAR para abortar) ---\n";
    if (!pedirTexto("Codigo: ", r.codigo, sizeof r.codigo, nullptr, "")) {
        cout << "Registro cancelado. No se creo ningun repuesto.\n"; return;
    }
    Repuesto existente;
    CodigoError e = obtenerRepuestoPorCodigo(r.codigo, existente);
    if (e == E_OK) { cout << "ERROR: El codigo '" << r.codigo << "' ya esta registrado.\n"; return; }
    if (e != E_REPUESTO_NO_EXISTE) { mostrarErrorSistema(e); return; }

    if (!pedirTexto("Nombre: ", r.nombre, sizeof r.nombre, nullptr, "") ||
        !pedirTexto("Marca: ", r.marca, sizeof r.marca, nullptr, "") ||
        !pedirDecimal("Precio unitario (> 0): ", &r.precio, 0, true) ||
        !pedirEntero("Stock inicial (>= 0): ", &r.stock, 0, INT_MAX) ||
        !pedirEntero("Stock minimo (>= 0): ", &r.stockMinimo, 0, INT_MAX)) {
        cout << "Registro cancelado. No se creo ningun repuesto.\n";
        return;
    }
    if (!pedirConfirmacion("Guardar el repuesto")) {
        cout << "Registro cancelado. No se creo ningun repuesto.\n";
        return;
    }
    int idNuevo = 0;
    e = registrarRepuesto(r, &idNuevo);
    if (e == E_OK) cout << "EXITO: Repuesto '" << r.nombre << "' (" << r.codigo << ") registrado con ID " << idNuevo << ".\n";
    else if (e == E_DUPLICADO) cout << "ERROR: El codigo '" << r.codigo << "' ya esta registrado.\n";
    else if (e == E_DATOS_INVALIDOS) cout << "ERROR: No se pudo registrar el repuesto '" << r.nombre << "' (datos invalidos).\n";
    else mostrarErrorSistema(e);
}

void menuBuscarRepuesto() {
    const char* ops[2] = {"Buscar por ID", "Buscar por codigo o nombre (parcial)"};
    imprimirMenu("BUSCAR REPUESTO", nullptr, ops, 2, "Volver");
    int op;
    if (!pedirEntero("Seleccione una opcion: ", &op, 0, 2) || op == 0) return;
    if (op == 1) {
        int id;
        if (!pedirEntero("ID del repuesto: ", &id, 1, INT_MAX)) return;
        Repuesto r;
        CodigoError e = obtenerRepuestoPorID(id, r);
        if (e == E_OK) { cout << "\nRepuesto encontrado:\n"; mostrarRepuesto(r); }
        else if (e == E_ELIMINADO) cout << "El repuesto con ID " << id << " ('" << r.nombre << "') fue ELIMINADO.\n";
        else if (e == E_ID_NO_EXISTE) cout << "ERROR: No existe un repuesto con ID " << id << ".\n";
        else mostrarErrorSistema(e);
    } else {
        char texto[100];
        if (!pedirTexto("Codigo o nombre (o parte): ", texto, sizeof texto, nullptr, "")) return;
        Repuesto res[MAX_RESULTADOS];
        int cantidad = 0;
        bool hayMas = false;
        CodigoError e = buscarRepuestosPorTexto(texto, res, cantidad, hayMas);
        if (e != E_OK) mostrarErrorSistema(e);
        else if (cantidad == 0) cout << "No se encontraron repuestos que contengan '" << texto << "'.\n";
        else { cout << "\n"; mostrarListaRepuestos(res, cantidad, hayMas); }
    }
}

void menuEntradaMercancia() {
    cout << "\n--- ENTRADA DE MERCANCIA (escriba CANCELAR para abortar) ---\n";
    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto r;
    CodigoError e = obtenerRepuestoPorCodigo(codigo, r);
    if (e == E_REPUESTO_NO_EXISTE) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    int cant;
    if (!pedirEntero("Cantidad a ingresar (> 0): ", &cant, 1, INT_MAX)) return;
    if (!pedirConfirmacion("Registrar la entrada")) { cout << "Entrada cancelada.\n"; return; }
    e = registrarEntradaMercancia(r.id, cant);
    if (e == E_OK) {
        obtenerRepuestoPorID(r.id, r);   // relee el stock actualizado desde el disco
        cout << "EXITO: Entrada registrada. Stock de '" << r.nombre << "': " << r.stock << ".\n";
    } else if (e == E_CANTIDAD_INVALIDA) cout << "ERROR: La cantidad debe ser mayor que 0.\n";
    else mostrarErrorSistema(e);
}

void menuListarRepuestos() {
    Repuesto res[MAX_RESULTADOS];
    int cantidad = 0;
    bool hayMas = false;
    CodigoError e = listarRepuestos(res, cantidad, hayMas);
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cantidad == 0) { cout << "\nNo hay repuestos registrados.\n"; return; }
    cout << "\n";
    mostrarListaRepuestos(res, cantidad, hayMas);
}

void menuCambiarPrecio() {
    cout << "\n--- CAMBIAR PRECIO (escriba CANCELAR para abortar) ---\n";
    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto r;
    CodigoError e = obtenerRepuestoPorCodigo(codigo, r);
    if (e == E_REPUESTO_NO_EXISTE) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    printf("Precio actual de '%s': %.2f\n", r.nombre, r.precio);
    float nuevo;
    if (!pedirDecimal("Nuevo precio (> 0): ", &nuevo, 0, true)) return;
    if (!pedirConfirmacion("Actualizar el precio")) { cout << "Cambio cancelado.\n"; return; }
    e = cambiarPrecioRepuesto(r.id, nuevo);
    if (e == E_OK) printf("EXITO: Nuevo precio de '%s': %.2f (las ordenes ya registradas no cambian).\n", r.nombre, nuevo);
    else if (e == E_PRECIO_INVALIDO) cout << "ERROR: El precio debe ser mayor que 0.\n";
    else mostrarErrorSistema(e);
}

void menuEliminarRepuesto() {
    cout << "\n--- ELIMINAR REPUESTO (escriba CANCELAR para abortar) ---\n";
    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto r;
    CodigoError e = obtenerRepuestoPorCodigo(codigo, r);
    if (e == E_REPUESTO_NO_EXISTE) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (!pedirConfirmacion("Eliminar este repuesto (borrado logico)")) { cout << "Eliminacion cancelada.\n"; return; }
    e = eliminarRepuesto(r.id);
    if (e == E_OK) cout << "EXITO: Repuesto '" << r.nombre << "' eliminado.\n";
    else if (e == E_REPUESTO_EN_USO)
        cout << "ERROR: El repuesto '" << r.nombre << "' aparece en ordenes de servicio y no puede eliminarse.\n";
    else mostrarErrorSistema(e);
}

void menuGestionInventario() {
    const char* ops[6] = {"Registrar repuesto", "Buscar repuesto", "Registrar entrada de mercancia",
                          "Listar repuestos", "Cambiar precio", "Eliminar repuesto"};
    int op;
    do {
        imprimirMenu("INVENTARIO DE REPUESTOS", nullptr, ops, 6, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 6)) return;
        switch (op) {
            case 1: menuRegistrarRepuesto(); break;
            case 2: menuBuscarRepuesto(); break;
            case 3: menuEntradaMercancia(); break;
            case 4: menuListarRepuestos(); break;
            case 5: menuCambiarPrecio(); break;
            case 6: menuEliminarRepuesto(); break;
        }
    } while (op != 0);
}

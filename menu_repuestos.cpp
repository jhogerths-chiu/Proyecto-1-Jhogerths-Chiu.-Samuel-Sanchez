// CAPA DE PRESENTACION - Inventario de repuestos.
#include "taller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
using namespace std;

void mostrarRepuesto(Repuesto* r) {
    if (r == nullptr) return;
    cout << "  Codigo       : " << r->codigo << "\n"
         << "  Nombre       : " << r->nombre << "\n"
         << "  Marca        : " << r->marca << "\n";
    printf("  Precio       : %.2f\n", r->precio);
    cout << "  Stock        : " << r->stock << " (minimo " << r->stockMinimo << ")"
         << (estaBajoStock(r) ? "  *** BAJO STOCK ***" : "") << "\n"
         << "  Registrado   : " << r->fechaRegistro << "\n";
}

void mostrarListaRepuestos(Repuesto** repuestos, int cantidad) {
    int w[8] = {4, 10, 28, 12, 9, 6, 6, 10};
    lineaBorde(w, 8, '-');
    printf("| %-4s | %-10s | %-28s | %-12s | %-9s | %-6s | %-6s | %-10s |\n",
           "ID", "Codigo", "Repuesto", "Marca", "Precio", "Stock", "Minimo", "Estado");
    lineaBorde(w, 8, '-');
    for (int i = 0; i < cantidad; i++) {
        Repuesto* r = repuestos[i];
        printf("| %-4d | %-10.10s | %-28.28s | %-12.12s | %9.2f | %-6d | %-6d | %-10s |\n",
               r->id, r->codigo, r->nombre, r->marca, r->precio, r->stock, r->stockMinimo,
               estaBajoStock(r) ? "BAJO STOCK" : "");
    }
    lineaBorde(w, 8, '-');
    printf("Total de repuestos: %d\n", cantidad);
}

void menuRegistrarRepuesto(SistemaTaller* s) {
    Repuesto r;
    memset(&r, 0, sizeof r);
    cout << "\n--- REGISTRAR REPUESTO (escriba CANCELAR para abortar) ---\n";
    if (!pedirTexto("Codigo: ", r.codigo, sizeof r.codigo, nullptr, "")) {
        cout << "Registro cancelado. No se creo ningun repuesto.\n"; return;
    }
    if (buscarRepuestoPorCodigo(s, r.codigo) != nullptr) {
        cout << "ERROR: El codigo '" << r.codigo << "' ya esta registrado.\n";
        return;
    }
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
    CodigoError err;
    Repuesto* creado = agregarRepuesto(s, r, &err);
    if (creado != nullptr) cout << "EXITO: Repuesto '" << creado->nombre << "' (" << creado->codigo << ") registrado.\n";
    else if (err == E_DUPLICADO) cout << "ERROR: El codigo '" << r.codigo << "' ya esta registrado.\n";
    else cout << "ERROR: No se pudo registrar el repuesto '" << r.nombre << "' (datos invalidos).\n";
}

void menuBuscarRepuesto(SistemaTaller* s) {
    const char* ops[2] = {"Buscar por ID", "Buscar por codigo o nombre (parcial)"};
    imprimirMenu("BUSCAR REPUESTO", nullptr, ops, 2, "Volver");
    int op;
    if (!pedirEntero("Seleccione una opcion: ", &op, 0, 2) || op == 0) return;
    if (op == 1) {
        int id;
        if (!pedirEntero("ID del repuesto: ", &id, 1, INT_MAX)) return;
        Repuesto* r = buscarRepuestoPorID(s, id);
        if (r == nullptr) cout << "ERROR: No existe un repuesto con ID " << id << ".\n";
        else { cout << "\nRepuesto encontrado:\n"; mostrarRepuesto(r); }
    } else {
        char texto[100];
        if (!pedirTexto("Codigo o nombre (o parte): ", texto, sizeof texto, nullptr, "")) return;
        int cantidad = 0;
        Repuesto** res = buscarRepuestosPorTexto(s, texto, &cantidad);
        if (cantidad == 0) cout << "No se encontraron repuestos que contengan '" << texto << "'.\n";
        else { cout << "\n"; mostrarListaRepuestos(res, cantidad); }
        delete[] res;
    }
}

void menuEntradaMercancia(SistemaTaller* s) {
    cout << "\n--- ENTRADA DE MERCANCIA (escriba CANCELAR para abortar) ---\n";
    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto* r = buscarRepuestoPorCodigo(s, codigo);
    if (r == nullptr) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    int cant;
    if (!pedirEntero("Cantidad a ingresar (> 0): ", &cant, 1, INT_MAX)) return;
    if (!pedirConfirmacion("Registrar la entrada")) { cout << "Entrada cancelada.\n"; return; }
    if (registrarEntradaMercancia(s, r->id, cant) == E_OK)
        cout << "EXITO: Entrada registrada. Stock de '" << r->nombre << "': " << r->stock << ".\n";
    else
        cout << "ERROR: No se pudo registrar la entrada de '" << r->nombre << "'.\n";
}

void menuListarRepuestos(SistemaTaller* s) {
    int cantidad = 0;
    Repuesto** lista = listarRepuestos(s, &cantidad);
    if (cantidad == 0) { cout << "\nNo hay repuestos registrados.\n"; return; }
    cout << "\n";
    mostrarListaRepuestos(lista, cantidad);
    delete[] lista;
}

void menuCambiarPrecio(SistemaTaller* s) {
    cout << "\n--- CAMBIAR PRECIO (escriba CANCELAR para abortar) ---\n";
    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto* r = buscarRepuestoPorCodigo(s, codigo);
    if (r == nullptr) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    printf("Precio actual de '%s': %.2f\n", r->nombre, r->precio);
    float nuevo;
    if (!pedirDecimal("Nuevo precio (> 0): ", &nuevo, 0, true)) return;
    if (!pedirConfirmacion("Actualizar el precio")) { cout << "Cambio cancelado.\n"; return; }
    if (cambiarPrecioRepuesto(s, r->id, nuevo) == E_OK)
        printf("EXITO: Nuevo precio de '%s': %.2f (las ordenes ya registradas no cambian).\n", r->nombre, r->precio);
    else
        cout << "ERROR: No se pudo cambiar el precio de '" << r->nombre << "'.\n";
}

void menuEliminarRepuesto(SistemaTaller* s) {
    cout << "\n--- ELIMINAR REPUESTO (escriba CANCELAR para abortar) ---\n";
    char codigo[20];
    if (!pedirTexto("Codigo del repuesto: ", codigo, sizeof codigo, nullptr, "")) return;
    Repuesto* r = buscarRepuestoPorCodigo(s, codigo);
    if (r == nullptr) { cout << "ERROR: No existe un repuesto con el codigo '" << codigo << "'.\n"; return; }
    if (repuestoUsadoEnOrdenes(s, r->id)) {
        cout << "ERROR: El repuesto '" << r->nombre << "' aparece en ordenes de servicio y no puede eliminarse.\n";
        return;
    }
    char nombre[100];
    copiarTexto(nombre, r->nombre, sizeof nombre);
    if (!pedirConfirmacion("Eliminar definitivamente este repuesto")) { cout << "Eliminacion cancelada.\n"; return; }
    if (eliminarRepuesto(s, r->id) == E_OK) cout << "EXITO: Repuesto '" << nombre << "' eliminado.\n";
    else cout << "ERROR: No se pudo eliminar el repuesto '" << nombre << "'.\n";
}

void menuGestionInventario(SistemaTaller* s) {
    const char* ops[7] = {"Registrar repuesto", "Buscar repuesto", "Registrar entrada de mercancia",
                          "Listar repuestos", "Cambiar precio", "Eliminar repuesto"};
    int op;
    do {
        imprimirMenu("INVENTARIO DE REPUESTOS", nullptr, ops, 6, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 6)) return;
        switch (op) {
            case 1: menuRegistrarRepuesto(s); break;
            case 2: menuBuscarRepuesto(s); break;
            case 3: menuEntradaMercancia(s); break;
            case 4: menuListarRepuestos(s); break;
            case 5: menuCambiarPrecio(s); break;
            case 6: menuEliminarRepuesto(s); break;
        }
    } while (op != 0);
}

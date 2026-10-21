#include "taller.h"
#include <iostream>
#include <cstring>

using namespace std;

// Buscar repuesto por código único
Repuesto* buscarRepuestoPorCodigo(SistemaTaller* s, const char* codigo) {
    for (int i = 0; i < s->numRepuestos; i++) {
        if (strcmp(s->repuestos[i].codigo, codigo) == 0) {
            return &(s->repuestos[i]);
        }
    }
    return nullptr;
}

// Agregar repuesto con redimensionamiento dinámico 2x
Repuesto* agregarRepuesto(SistemaTaller* s, Repuesto r) {
    if (buscarRepuestoPorCodigo(s, r.codigo) != nullptr) return nullptr;
    if (r.precioUnitario < 0 || r.stock < 0) return nullptr;

    if (s->numRepuestos >= s->capacidadRepuestos) {
        int nuevaCapacidad = s->capacidadRepuestos * 2;
        Repuesto* nuevoArreglo = new Repuesto[nuevaCapacidad];
        for (int i = 0; i < s->numRepuestos; i++) {
            nuevoArreglo[i] = s->repuestos[i];
        }
        delete[] s->repuestos;
        s->repuestos = nuevoArreglo;
        s->capacidadRepuestos = nuevaCapacidad;
    }

    r.id = s->siguienteIdRepuesto++;
    s->repuestos[s->numRepuestos] = r;
    s->numRepuestos++;
    return &(s->repuestos[s->numRepuestos - 1]);
}

// Menú interactivo de registro de repuesto
void menuRegistrarRepuesto(SistemaTaller* s) {
    Repuesto r;
    cout << "\n--- REGISTRAR REPUESTO ---\n";
    cout << "Codigo del repuesto: ";
    cin >> r.codigo;
    cin.ignore();
    cout << "Descripcion: ";
    cin.getline(r.descripcion, 100);
    cout << "Precio Unitario ($): ";
    cin >> r.precioUnitario;
    cout << "Cantidad en Stock: ";
    cin >> r.stock;

    if (agregarRepuesto(s, r) != nullptr) {
        cout << "EXITO: Repuesto registrado en el inventario.\n";
    } else {
        cout << "ERROR: Codigo duplicado o valores invalidos.\n";
    }
}

// Listar inventario de repuestos con alerta de bajo stock (<= 3)
void listarRepuestos(const SistemaTaller* s) {
    cout << "\n--- INVENTARIO DE REPUESTOS ---\n";
    if (s->numRepuestos == 0) {
        cout << "No hay repuestos registrados en el inventario.\n";
        return;
    }
    for (int i = 0; i < s->numRepuestos; i++) {
        cout << "ID: " << s->repuestos[i].id
             << " | Codigo: " << s->repuestos[i].codigo
             << " | Descripcion: " << s->repuestos[i].descripcion
             << " | Precio: $" << s->repuestos[i].precioUnitario
             << " | Stock: " << s->repuestos[i].stock;
        
        // Alerta si el stock es bajo
        if (s->repuestos[i].stock <= 3) {
            cout << "  <-- [ALERTA: BAJO STOCK]";
        }
        cout << "\n";
    }
    cout << "Total de items en inventario: " << s->numRepuestos << "\n";
}
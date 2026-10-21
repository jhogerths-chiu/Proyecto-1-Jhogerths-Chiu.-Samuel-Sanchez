#include "taller.h"
#include <iostream>
#include <cstring>
#include <limits>

using namespace std;

// Buscar mecánico por cédula para evitar duplicados
Mecanico* buscarMecanicoPorCedula(SistemaTaller* s, const char* cedula) {
    for (int i = 0; i < s->numMecanicos; i++) {
        if (strcmp(s->mecanicos[i].cedula, cedula) == 0) {
            return &(s->mecanicos[i]);
        }
    }
    return nullptr;
}

// Agregar mecánico con redimensionamiento 2x
Mecanico* agregarMecanico(SistemaTaller* s, Mecanico m) {
    if (buscarMecanicoPorCedula(s, m.cedula) != nullptr) return nullptr;

    if (s->numMecanicos >= s->capacidadMecanicos) {
        int nuevaCapacidad = s->capacidadMecanicos * 2;
        Mecanico* nuevoArreglo = new Mecanico[nuevaCapacidad];
        for (int i = 0; i < s->numMecanicos; i++) {
            nuevoArreglo[i] = s->mecanicos[i];
        }
        delete[] s->mecanicos;
        s->mecanicos = nuevoArreglo;
        s->capacidadMecanicos = nuevaCapacidad;
    }

    m.id = s->siguienteIdMecanico++;
    m.ordenesActivas = 0; // Inicialmente sin órdenes
    s->mecanicos[s->numMecanicos] = m;
    s->numMecanicos++;
    return &(s->mecanicos[s->numMecanicos - 1]);
}

// Menú interactivo de registro
void menuRegistrarMecanico(SistemaTaller* s) {
    Mecanico m;
    cout << "\n--- REGISTRAR MECANICO ---\n";
    cout << "Cedula: ";
    cin >> m.cedula;
    cin.ignore();
    cout << "Nombre completo: ";
    cin.getline(m.nombre, 100);
    cout << "Especialidad (ej. Motor, Electricidad): ";
    cin.getline(m.especialidad, 50);

    if (agregarMecanico(s, m) != nullptr) {
        cout << "EXITO: Mecanico registrado correctamente.\n";
    } else {
        cout << "ERROR: Ya existe un mecanico registrado con esa cedula.\n";
    }
}

// Listar mecánicos registrados
void listarMecanicos(const SistemaTaller* s) {
    cout << "\n--- LISTADO DE MECANICOS ---\n";
    if (s->numMecanicos == 0) {
        cout << "No hay mecanicos registrados.\n";
        return;
    }
    for (int i = 0; i < s->numMecanicos; i++) {
        cout << "ID: " << s->mecanicos[i].id
             << " | Cedula: " << s->mecanicos[i].cedula
             << " | Nombre: " << s->mecanicos[i].nombre
             << " | Especialidad: " << s->mecanicos[i].especialidad
             << " | Ordenes Activas: " << s->mecanicos[i].ordenesActivas << "\n";
    }
    cout << "Total de mecanicos: " << s->numMecanicos << "\n";
}
// CAPA DE PRESENTACION - lectura robusta de datos y dibujo de menus/tablas.
// Todas las funciones pedir* aceptan "CANCELAR" y detectan el fin de la entrada
// (necesario para ejecutar con ./taller < pruebas/entrada.txt).
#include "taller.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <climits>
using namespace std;

bool entradaTerminada = false;

static bool leerLinea(string& linea) {
    if (!getline(cin, linea)) {
        if (!entradaTerminada) cout << "\n[Fin de la entrada]\n";
        entradaTerminada = true;
        return false;
    }
    size_t a = linea.find_first_not_of(" \t\r\n");
    if (a == string::npos) { linea = ""; return true; }
    size_t b = linea.find_last_not_of(" \t\r\n");
    linea = linea.substr(a, b - a + 1);
    return true;
}

static bool esCancelar(const string& l) {
    return igualesIgnorandoMayusculas(l.c_str(), "CANCELAR");
}

bool pedirTexto(const char* prompt, char* dest, int tam,
                bool (*valida)(const char*), const char* msgInvalido) {
    string l;
    while (true) {
        cout << prompt << flush;
        if (!leerLinea(l)) return false;
        if (esCancelar(l)) { cout << "Operacion cancelada.\n"; return false; }
        if (l.empty()) { cout << "ERROR: Este campo no puede estar vacio.\n"; continue; }
        if ((int)l.size() >= tam) {
            cout << "ERROR: Maximo " << (tam - 1) << " caracteres.\n";
            continue;
        }
        if (valida != nullptr && !valida(l.c_str())) { cout << msgInvalido << "\n"; continue; }
        copiarTexto(dest, l.c_str(), tam);
        return true;
    }
}

bool pedirEntero(const char* prompt, int* out, int min, int max) {
    string l;
    while (true) {
        cout << prompt << flush;
        if (!leerLinea(l)) return false;
        if (esCancelar(l)) { cout << "Operacion cancelada.\n"; return false; }
        char* fin = nullptr;
        errno = 0;
        long v = strtol(l.c_str(), &fin, 10);
        if (l.empty() || *fin != '\0' || errno == ERANGE) {
            cout << "ERROR: '" << l << "' no es un numero entero valido.\n";
            continue;
        }
        if (v < min || v > max) {
            if (max == INT_MAX) cout << "ERROR: El valor debe ser mayor o igual a " << min << ".\n";
            else cout << "ERROR: El valor debe estar entre " << min << " y " << max << ".\n";
            continue;
        }
        *out = (int)v;
        return true;
    }
}

bool pedirDecimal(const char* prompt, float* out, float min, bool estricto) {
    string l;
    while (true) {
        cout << prompt << flush;
        if (!leerLinea(l)) return false;
        if (esCancelar(l)) { cout << "Operacion cancelada.\n"; return false; }
        for (size_t i = 0; i < l.size(); i++) if (l[i] == ',') l[i] = '.';
        char* fin = nullptr;
        double v = strtod(l.c_str(), &fin);
        if (l.empty() || *fin != '\0') {
            cout << "ERROR: '" << l << "' no es un numero valido.\n";
            continue;
        }
        if (estricto ? (v <= min) : (v < min)) {
            cout << "ERROR: El valor debe ser " << (estricto ? "mayor que " : "mayor o igual a ") << min << ".\n";
            continue;
        }
        *out = (float)v;
        return true;
    }
}

bool pedirConfirmacion(const char* prompt) {
    string l;
    while (true) {
        cout << prompt << " (S/N): " << flush;
        if (!leerLinea(l)) return false;
        if (esCancelar(l)) { cout << "Operacion cancelada.\n"; return false; }
        if (l == "S" || l == "s") return true;
        if (l == "N" || l == "n") return false;
        cout << "Responda S o N.\n";
    }
}

void lineaBorde(const int* anchos, int n, char relleno) {
    putchar('+');
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < anchos[i] + 2; j++) putchar(relleno);
        putchar('+');
    }
    putchar('\n');
}

void imprimirMenu(const char* titulo, const char* subtitulo,
                  const char* const* opciones, int n, const char* textoCero) {
    int w[1] = {41};
    printf("\n");
    lineaBorde(w, 1, '=');
    printf("| %-41.41s |\n", titulo);
    if (subtitulo != nullptr) printf("| %-41.41s |\n", subtitulo);
    lineaBorde(w, 1, '=');
    for (int i = 0; i < n; i++) printf("| %d. %-38.38s |\n", i + 1, opciones[i]);
    printf("| 0. %-38.38s |\n", textoCero);
    lineaBorde(w, 1, '=');
}

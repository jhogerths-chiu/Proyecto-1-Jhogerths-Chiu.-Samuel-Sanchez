// Utilidades de la capa de logica: validaciones y manejo de texto. SIN cin/cout.
#include "taller.h"
#include <cstring>
#include <cctype>
#include <ctime>

void copiarTexto(char* dest, const char* src, int tam) {
    strncpy(dest, src, tam - 1);
    dest[tam - 1] = '\0';
}

void aMayusculas(char* t) {
    for (; *t; ++t) *t = (char)toupper((unsigned char)*t);
}

bool igualesIgnorandoMayusculas(const char* a, const char* b) {
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) return false;
        ++a; ++b;
    }
    return *a == *b;
}

bool contieneIgnorandoMayusculas(const char* texto, const char* sub) {
    int n = (int)strlen(sub);
    if (n == 0) return true;
    for (int i = 0; texto[i]; i++) {
        int j = 0;
        while (j < n && texto[i + j] &&
               toupper((unsigned char)texto[i + j]) == toupper((unsigned char)sub[j])) j++;
        if (j == n) return true;
    }
    return false;
}

bool textoNoVacio(const char* t) {
    for (; *t; ++t) if (!isspace((unsigned char)*t)) return true;
    return false;
}

bool cedulaValida(const char* c) {
    int i = 0;
    if (strlen(c) >= 2 && c[1] == '-') {
        char p = (char)toupper((unsigned char)c[0]);
        if (p == 'V' || p == 'E' || p == 'J') i = 2;
    }
    if (c[i] == '\0') return false;
    for (; c[i]; i++) if (!isdigit((unsigned char)c[i])) return false;
    return true;
}

bool placaValida(const char* p) {
    int n = (int)strlen(p);
    if (n < 6 || n > 7) return false;
    for (int i = 0; i < n; i++) if (!isalnum((unsigned char)p[i])) return false;
    return true;
}

bool fechaValida(const char* f) {
    if (strlen(f) != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) { if (f[i] != '-') return false; }
        else if (!isdigit((unsigned char)f[i])) return false;
    }
    int anio = (f[0]-'0')*1000 + (f[1]-'0')*100 + (f[2]-'0')*10 + (f[3]-'0');
    int mes  = (f[5]-'0')*10 + (f[6]-'0');
    int dia  = (f[8]-'0')*10 + (f[9]-'0');
    if (anio < 1900 || anio > 2100 || mes < 1 || mes > 12 || dia < 1) return false;
    int dias[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
    int maxDia = dias[mes - 1] + ((mes == 2 && bisiesto) ? 1 : 0);
    return dia <= maxDia;
}

bool especialidadValida(const char* e) {
    const char* validas[5] = {"MOTOR", "FRENOS", "ELECTRICIDAD", "SUSPENSION", "GENERAL"};
    for (int i = 0; i < 5; i++)
        if (igualesIgnorandoMayusculas(e, validas[i])) return true;
    return false;
}

void fechaActual(char* dest) {
    time_t t = time(nullptr);
    struct tm* lt = localtime(&t);
    strftime(dest, 11, "%Y-%m-%d", lt);
}

// CAPA DE PERSISTENCIA - Operaciones sobre el header, taller.bin y utilidades de disco.
// SIN cin/cout. Solo <fstream> (nada de fopen/fclose).
#include "taller.h"
#include <fstream>
#include <cstring>
#include <cerrno>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
using namespace std;

static char ultimoArchivoFallido[260] = "";

CodigoError errorArchivo(const char* ruta) {
    copiarTexto(ultimoArchivoFallido, ruta, sizeof ultimoArchivoFallido);
    return E_ARCHIVO;
}

CodigoError errorCorrupto(const char* ruta) {
    copiarTexto(ultimoArchivoFallido, ruta, sizeof ultimoArchivoFallido);
    return E_ARCHIVO_CORRUPTO;
}

const char* archivoConError() { return ultimoArchivoFallido; }

// posicion = sizeof(ArchivoHeader) + indice * sizeof(Estructura)
long posicionRegistro(size_t tamRegistro, int indice) {
    return (long)sizeof(ArchivoHeader) + (long)indice * (long)tamRegistro;
}

// ---------- Header ----------
CodigoError inicializarArchivo(const char* ruta) {
    {
        ifstream existente(ruta, ios::binary);
        if (existente.is_open()) {
            // Ya existe: NO se modifica. Solo se comprueba que tenga un header completo.
            ArchivoHeader h;
            existente.read(reinterpret_cast<char*>(&h), sizeof h);
            if (existente.gcount() != (streamsize)sizeof h) return errorCorrupto(ruta);
            return E_OK;
        }
    }
    ofstream nuevo(ruta, ios::binary);
    if (!nuevo) return errorArchivo(ruta);
    ArchivoHeader h;
    h.cantidadRegistros = 0;
    h.proximoID = 1;
    h.registrosActivos = 0;
    h.version = 1;
    nuevo.write(reinterpret_cast<const char*>(&h), sizeof h);
    nuevo.close();
    if (!nuevo) return errorArchivo(ruta);
    return E_OK;
}

CodigoError leerHeader(const char* ruta, ArchivoHeader& h) {
    ifstream f(ruta, ios::binary);
    if (!f) return errorArchivo(ruta);
    f.seekg(0);
    f.read(reinterpret_cast<char*>(&h), sizeof h);
    if (f.gcount() != (streamsize)sizeof h) return errorCorrupto(ruta);
    return E_OK;
}

// Sobrescribe unicamente la posicion 0: no toca los registros.
CodigoError escribirHeader(const char* ruta, const ArchivoHeader& h) {
    fstream f(ruta, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(ruta);
    f.seekp(0);
    f.write(reinterpret_cast<const char*>(&h), sizeof h);
    f.close();
    if (!f) return errorArchivo(ruta);
    return E_OK;
}

// ---------- taller.bin: un unico registro, sin header ----------
bool existeTaller() {
    ifstream f(RUTA_TALLER, ios::binary);
    if (!f) return false;
    f.seekg(0, ios::end);
    return (size_t)f.tellg() >= sizeof(Taller);
}

CodigoError leerTaller(Taller& t) {
    ifstream f(RUTA_TALLER, ios::binary);
    if (!f) return errorArchivo(RUTA_TALLER);
    f.read(reinterpret_cast<char*>(&t), sizeof t);
    if (f.gcount() != (streamsize)sizeof t) return errorCorrupto(RUTA_TALLER);
    return E_OK;
}

CodigoError guardarTaller(Taller& t) {
    time_t ahora = time(nullptr);
    if (t.fechaCreacion == 0) t.fechaCreacion = ahora;
    t.fechaUltimaModificacion = ahora;
    ofstream f(RUTA_TALLER, ios::binary | ios::trunc);
    if (!f) return errorArchivo(RUTA_TALLER);
    f.write(reinterpret_cast<const char*>(&t), sizeof t);
    f.close();
    if (!f) return errorArchivo(RUTA_TALLER);
    return E_OK;
}

// ---------- Utilidades de sistema ----------
bool crearCarpeta(const char* ruta) {
#ifdef _WIN32
    int r = _mkdir(ruta);
#else
    int r = mkdir(ruta, 0755);
#endif
    return r == 0 || errno == EEXIST;
}

void formatearFecha(time_t t, char* dest) {
    struct tm* lt = localtime(&t);
    if (lt == nullptr) { copiarTexto(dest, "?", 11); return; }
    strftime(dest, 11, "%Y-%m-%d", lt);
}

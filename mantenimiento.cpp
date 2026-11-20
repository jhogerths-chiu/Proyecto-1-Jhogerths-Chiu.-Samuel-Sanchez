// CAPA DE LOGICA - Mantenimiento: verificacion de integridad y respaldo. SIN cin/cout.
// verificarIntegridad() no imprime: llena un ReporteIntegridad y la presentacion lo muestra.
#include "taller.h"
#include <cstring>
#include <cstdio>
#include <cstdarg>

static void problema(ReporteIntegridad& r, const char* fmt, ...) {
    r.problemas++;
    if (r.numMensajes >= MAX_PROBLEMAS) return;
    va_list args;
    va_start(args, fmt);
    vsnprintf(r.mensajes[r.numMensajes], sizeof r.mensajes[0], fmt, args);
    va_end(args);
    r.numMensajes++;
}

// Un registro "no esta" si el ID no existe o si la lectura se sale del archivo real
static bool noEncontrado(CodigoError e) { return e == E_ID_NO_EXISTE || e == E_ARCHIVO_CORRUPTO; }

// Verifica el header de un archivo contra su contenido real. 'registros' = cuantos se pueden
// recorrer con seguridad (el menor entre lo que dice el header y lo que realmente hay).
static CodigoError verificarHeader(const char* ruta, const char* nombre, size_t tam,
                                   ArchivoHeader& h, int& registros, int& reales,
                                   ReporteIntegridad& r) {
    registros = 0;
    reales = 0;
    CodigoError e = leerHeader(ruta, h);
    if (e != E_OK) { problema(r, "ROTO: No se pudo leer el header de %s", nombre); return e; }
    e = contarRegistrosEnArchivo(ruta, tam, reales);
    if (e == E_ARCHIVO_CORRUPTO)
        problema(r, "ROTO: %s termina con un registro incompleto (tamano no es multiplo del registro)", nombre);
    else if (e != E_OK) { problema(r, "ROTO: No se pudo abrir %s", nombre); return e; }
    if (h.cantidadRegistros != reales)
        problema(r, "ROTO: Header de %s indica %d registros; el archivo contiene %d",
                 nombre, h.cantidadRegistros, reales);
    registros = h.cantidadRegistros < reales ? h.cantidadRegistros : reales;
    return E_OK;
}

static void verificarActivos(const char* nombre, const ArchivoHeader& h, int reales,
                             int activosContados, ReporteIntegridad& r) {
    if (h.cantidadRegistros == reales && h.registrosActivos != activosContados)
        problema(r, "ROTO: Header de %s indica %d activos; se contaron %d",
                 nombre, h.registrosActivos, activosContados);
}

CodigoError verificarIntegridad(ReporteIntegridad& r) {
    memset(&r, 0, sizeof r);
    ArchivoHeader h;
    int n = 0, reales = 0, activos = 0;

    // ---- vehiculos.bin ----
    if (verificarHeader(RUTA_VEHICULOS, "vehiculos.bin", sizeof(Vehiculo), h, n, reales, r) == E_OK) {
        activos = 0;
        for (int id = 1; id <= n; id++) {
            Vehiculo v;
            if (leerVehiculoPorID(id, v) != E_OK) break;
            r.vehiculosVerificados++;
            if (!v.eliminado) activos++;
        }
        verificarActivos("vehiculos.bin", h, reales, activos, r);
    }

    // ---- mecanicos.bin ----
    if (verificarHeader(RUTA_MECANICOS, "mecanicos.bin", sizeof(Mecanico), h, n, reales, r) == E_OK) {
        activos = 0;
        for (int id = 1; id <= n; id++) {
            Mecanico m;
            if (leerMecanicoPorID(id, m) != E_OK) break;
            r.mecanicosVerificados++;
            if (!m.eliminado) activos++;
        }
        verificarActivos("mecanicos.bin", h, reales, activos, r);
    }

    // ---- repuestos.bin (d: stock negativo) ----
    if (verificarHeader(RUTA_REPUESTOS, "repuestos.bin", sizeof(Repuesto), h, n, reales, r) == E_OK) {
        activos = 0;
        for (int id = 1; id <= n; id++) {
            Repuesto rep;
            if (leerRepuestoPorID(id, rep) != E_OK) break;
            r.repuestosVerificados++;
            if (!rep.eliminado) activos++;
            if (rep.stock < 0)
                problema(r, "ROTO: Repuesto #%d (%s) tiene stock negativo (%d)", rep.id, rep.codigo, rep.stock);
        }
        verificarActivos("repuestos.bin", h, reales, activos, r);
    }

    // ---- ordenes.bin (a, b, c) ----
    if (verificarHeader(RUTA_ORDENES, "ordenes.bin", sizeof(OrdenServicio), h, n, reales, r) == E_OK) {
        activos = 0;
        for (int id = 1; id <= n; id++) {
            OrdenServicio o;
            if (leerOrdenPorID(id, o) != E_OK) break;
            r.ordenesVerificadas++;
            if (!o.eliminado) activos++;
            bool abierta = strcmp(o.estado, "ABIERTA") == 0;

            Vehiculo v;
            CodigoError e = leerVehiculoPorID(o.idVehiculo, v);
            if (noEncontrado(e))
                problema(r, "ROTO: Orden #%d apunta al vehiculo ID %d (no existe en vehiculos.bin)", o.id, o.idVehiculo);
            else if (e == E_OK && abierta && v.eliminado)
                problema(r, "ROTO: Orden #%d ABIERTA apunta al vehiculo ID %d (%s), que esta eliminado",
                         o.id, o.idVehiculo, v.placa);

            Mecanico m;
            e = leerMecanicoPorID(o.idMecanico, m);
            if (noEncontrado(e))
                problema(r, "ROTO: Orden #%d apunta al mecanico ID %d (no existe en mecanicos.bin)", o.id, o.idMecanico);
            else if (e == E_OK && abierta && m.eliminado)
                problema(r, "ROTO: Orden #%d ABIERTA apunta al mecanico ID %d (%s), que esta dado de baja",
                         o.id, o.idMecanico, m.nombre);

            if (o.numLineas < 0 || o.numLineas > MAX_LINEAS_ORDEN) {
                problema(r, "ROTO: Orden #%d tiene un numero de lineas invalido (%d)", o.id, o.numLineas);
                continue;
            }
            for (int j = 0; j < o.numLineas; j++) {
                Repuesto rep;
                e = leerRepuestoPorID(o.lineas[j].idRepuesto, rep);
                if (noEncontrado(e))
                    problema(r, "ROTO: Orden #%d, linea %d: el repuesto ID %d no existe en repuestos.bin",
                             o.id, j + 1, o.lineas[j].idRepuesto);
            }
        }
        verificarActivos("ordenes.bin", h, reales, activos, r);
    }
    return E_OK;
}

// ---------- Respaldo ----------
CodigoError crearBackup(char* rutaBackup, int tam) {
    time_t t = time(nullptr);
    struct tm* lt = localtime(&t);
    char carpeta[80];
    strftime(carpeta, sizeof carpeta, "backup_%Y-%m-%d_%H-%M", lt);
    if (!crearCarpeta(carpeta)) return errorArchivo(carpeta);

    const char* origenes[5] = {RUTA_TALLER, RUTA_VEHICULOS, RUTA_MECANICOS, RUTA_REPUESTOS, RUTA_ORDENES};
    const char* nombres[5]  = {"taller.bin", "vehiculos.bin", "mecanicos.bin", "repuestos.bin", "ordenes.bin"};
    for (int i = 0; i < 5; i++) {
        char destino[200];
        snprintf(destino, sizeof destino, "%s/%s", carpeta, nombres[i]);
        CodigoError e = copiarArchivo(origenes[i], destino);
        if (e != E_OK) return e;
    }
    copiarTexto(rutaBackup, carpeta, tam);
    return E_OK;
}

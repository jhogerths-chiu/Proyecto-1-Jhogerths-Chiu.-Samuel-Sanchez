// Arranque del sistema: prepara la carpeta datos/ y los archivos binarios. SIN cin/cout.
// Ya no existe SistemaTaller ni arreglos en RAM: los datos viven en disco.
#include "taller.h"

CodigoError inicializarSistema() {
    if (!crearCarpeta(CARPETA_DATOS)) return errorArchivo(CARPETA_DATOS);

    // Cada archivo de entidades que no exista se crea con su header inicial.
    // taller.bin NO se crea aqui: requiere pedir los datos al usuario (lo hace main).
    const char* rutas[4] = {RUTA_VEHICULOS, RUTA_MECANICOS, RUTA_REPUESTOS, RUTA_ORDENES};
    for (int i = 0; i < 4; i++) {
        CodigoError e = inicializarArchivo(rutas[i]);
        if (e != E_OK) return e;
    }
    return E_OK;
}

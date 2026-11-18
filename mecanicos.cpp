// CAPA DE LOGICA - Mecanicos. SIN cin/cout.
#include "taller.h"
#include <cstring>

// E_ELIMINADO = mecanico dado de baja. El registro queda en 'out' para poder mostrar su
// nombre en el historial (ordenes CERRADAS y ANULADAS).
CodigoError obtenerMecanicoPorID(int id, Mecanico& out, long* posicion) {
    CodigoError e = leerMecanicoPorID(id, out, posicion);
    if (e != E_OK) return e;
    return out.eliminado ? E_ELIMINADO : E_OK;
}

CodigoError registrarMecanico(Mecanico datos, int* idNuevo) {
    if (!textoNoVacio(datos.nombre) || !cedulaValida(datos.cedula) ||
        !especialidadValida(datos.especialidad) || !fechaValida(datos.fechaIngreso))
        return E_DATOS_INVALIDOS;

    // Cedula unica entre TODOS los registros (activos y dados de baja)
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_MECANICOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        Mecanico m;
        e = leerMecanicoPorID(id, m);
        if (e != E_OK) return e;
        if (igualesIgnorandoMayusculas(m.cedula, datos.cedula)) return E_DUPLICADO;
    }

    aMayusculas(datos.cedula);
    aMayusculas(datos.especialidad);
    e = guardarMecanico(datos);
    if (e == E_OK && idNuevo != nullptr) *idNuevo = datos.id;
    return e;
}

// Activos y dados de baja (la presentacion los distingue por m.eliminado)
CodigoError listarMecanicos(Mecanico* res, int& cantidad, bool& hayMas) {
    cantidad = 0;
    hayMas = false;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_MECANICOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        if (cantidad == MAX_RESULTADOS) { hayMas = true; break; }
        e = leerMecanicoPorID(id, res[cantidad]);
        if (e != E_OK) return e;
        cantidad++;
    }
    return E_OK;
}

CodigoError darDeBajaMecanico(int id) {
    Mecanico m;
    CodigoError e = obtenerMecanicoPorID(id, m);     // E_ID_NO_EXISTE o E_ELIMINADO (ya de baja)
    if (e != E_OK) return e;
    int abiertas = 0;
    e = contarOrdenesAbiertasMecanico(id, abiertas);
    if (e != E_OK) return e;
    if (abiertas > 0) return E_MECANICO_CON_ORDENES_ABIERTAS;
    return marcarMecanicoEliminado(id);
}

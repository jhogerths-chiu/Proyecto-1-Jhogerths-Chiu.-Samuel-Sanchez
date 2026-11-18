// CAPA DE LOGICA - Ordenes de servicio. SIN cin/cout.
// (Commit 1: solo las consultas que necesitan las reglas de vehiculos, mecanicos y repuestos.
//  Abrir, agregar/quitar repuesto, cerrar, anular y totales se agregan en el commit 2.)
#include "taller.h"
#include <cstring>

// Cuenta las ordenes del vehiculo en CUALQUIER estado. Un solo registro en memoria a la vez.
CodigoError contarOrdenesDeVehiculo(int idVehiculo, int& cantidad) {
    cantidad = 0;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) return e;
        if (o.idVehiculo == idVehiculo) cantidad++;
    }
    return E_OK;
}

CodigoError contarOrdenesAbiertasMecanico(int idMecanico, int& cantidad) {
    cantidad = 0;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) return e;
        if (o.idMecanico == idMecanico && strcmp(o.estado, "ABIERTA") == 0) cantidad++;
    }
    return E_OK;
}

// El repuesto aparece en el detalle de alguna orden (de cualquier estado, incluso ANULADA)
CodigoError repuestoUsadoEnOrdenes(int idRepuesto, bool& usado) {
    usado = false;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros && !usado; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) return e;
        for (int j = 0; j < o.numLineas; j++)
            if (o.lineas[j].idRepuesto == idRepuesto) { usado = true; break; }
    }
    return E_OK;
}

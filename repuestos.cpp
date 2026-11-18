// CAPA DE LOGICA - Repuestos (inventario). SIN cin/cout.
#include "taller.h"
#include <cstring>

bool estaBajoStock(const Repuesto& r) {
    return r.stock <= r.stockMinimo;
}

// Solo activos: un repuesto eliminado retorna E_ELIMINADO (con el registro en 'out')
CodigoError obtenerRepuestoPorID(int id, Repuesto& out, long* posicion) {
    CodigoError e = leerRepuestoPorID(id, out, posicion);
    if (e != E_OK) return e;
    return out.eliminado ? E_ELIMINADO : E_OK;
}

CodigoError obtenerRepuestoPorCodigo(const char* codigo, Repuesto& out) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        Repuesto r;
        e = leerRepuestoPorID(id, r);
        if (e != E_OK) return e;
        if (!r.eliminado && igualesIgnorandoMayusculas(r.codigo, codigo)) { out = r; return E_OK; }
    }
    return E_REPUESTO_NO_EXISTE;
}

CodigoError registrarRepuesto(Repuesto datos, int* idNuevo) {
    if (!textoNoVacio(datos.codigo) || !textoNoVacio(datos.nombre) || !textoNoVacio(datos.marca) ||
        datos.precio <= 0 || datos.stock < 0 || datos.stockMinimo < 0)
        return E_DATOS_INVALIDOS;

    // Codigo unico entre los ACTIVOS
    Repuesto existente;
    CodigoError e = obtenerRepuestoPorCodigo(datos.codigo, existente);
    if (e == E_OK) return E_DUPLICADO;
    if (e != E_REPUESTO_NO_EXISTE) return e;

    aMayusculas(datos.codigo);
    e = guardarRepuesto(datos);
    if (e == E_OK && idNuevo != nullptr) *idNuevo = datos.id;
    return e;
}

CodigoError buscarRepuestosPorTexto(const char* subcadena, Repuesto* res, int& cantidad, bool& hayMas) {
    cantidad = 0;
    hayMas = false;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        Repuesto r;
        e = leerRepuestoPorID(id, r);
        if (e != E_OK) return e;
        if (r.eliminado) continue;
        if (!contieneIgnorandoMayusculas(r.codigo, subcadena) &&
            !contieneIgnorandoMayusculas(r.nombre, subcadena)) continue;
        if (cantidad == MAX_RESULTADOS) { hayMas = true; break; }
        res[cantidad++] = r;
    }
    return E_OK;
}

CodigoError listarRepuestos(Repuesto* res, int& cantidad, bool& hayMas) {
    cantidad = 0;
    hayMas = false;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        Repuesto r;
        e = leerRepuestoPorID(id, r);
        if (e != E_OK) return e;
        if (r.eliminado) continue;
        if (cantidad == MAX_RESULTADOS) { hayMas = true; break; }
        res[cantidad++] = r;
    }
    return E_OK;
}

CodigoError registrarEntradaMercancia(int idRepuesto, int cantidad) {
    Repuesto r;
    CodigoError e = obtenerRepuestoPorID(idRepuesto, r);
    if (e != E_OK) return e;
    if (cantidad <= 0) return E_CANTIDAD_INVALIDA;
    r.stock += cantidad;
    return sobrescribirRepuesto(r);
}

CodigoError cambiarPrecioRepuesto(int idRepuesto, float nuevoPrecio) {
    Repuesto r;
    CodigoError e = obtenerRepuestoPorID(idRepuesto, r);
    if (e != E_OK) return e;
    if (nuevoPrecio <= 0) return E_PRECIO_INVALIDO;
    r.precio = nuevoPrecio;       // las ordenes existentes conservan su precioUnitario
    return sobrescribirRepuesto(r);
}

CodigoError eliminarRepuesto(int idRepuesto) {
    Repuesto r;
    CodigoError e = obtenerRepuestoPorID(idRepuesto, r);
    if (e != E_OK) return e;
    bool usado = false;
    e = repuestoUsadoEnOrdenes(idRepuesto, usado);
    if (e != E_OK) return e;
    if (usado) return E_REPUESTO_EN_USO;
    return marcarRepuestoEliminado(idRepuesto);
}

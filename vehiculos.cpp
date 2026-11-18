// CAPA DE LOGICA - Vehiculos. SIN cin/cout. Lee y escribe solo a traves de la persistencia.
#include "taller.h"
#include <cstring>

static bool datosEditablesValidos(const Vehiculo& d) {
    return textoNoVacio(d.marca) && textoNoVacio(d.modelo) &&
           textoNoVacio(d.nombrePropietario) &&
           cedulaValida(d.cedulaPropietario) &&
           textoNoVacio(d.telefonoPropietario) &&
           d.anio >= 1950 && d.anio <= 2027;
}

// Busqueda por ID: calculo directo de la posicion, sin recorrer el archivo.
// E_ELIMINADO deja el registro en 'out' para que la presentacion pueda decir "fue eliminado".
CodigoError obtenerVehiculoPorID(int id, Vehiculo& out, long* posicion) {
    CodigoError e = leerVehiculoPorID(id, out, posicion);
    if (e != E_OK) return e;
    return out.eliminado ? E_ELIMINADO : E_OK;
}

// Busqueda por otro campo: recorre el archivo con UN solo registro en memoria a la vez.
CodigoError obtenerVehiculoPorPlaca(const char* placa, Vehiculo& out) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        Vehiculo v;
        e = leerVehiculoPorID(id, v);
        if (e != E_OK) return e;
        if (!v.eliminado && igualesIgnorandoMayusculas(v.placa, placa)) { out = v; return E_OK; }
    }
    return E_VEHICULO_NO_EXISTE;
}

CodigoError registrarVehiculo(Vehiculo datos, int* idNuevo) {
    if (!placaValida(datos.placa) || !datosEditablesValidos(datos) || datos.kilometraje < 0)
        return E_DATOS_INVALIDOS;

    // Placa unica solo entre los ACTIVOS (la de un vehiculo eliminado puede reutilizarse)
    Vehiculo existente;
    CodigoError e = obtenerVehiculoPorPlaca(datos.placa, existente);
    if (e == E_OK) return E_DUPLICADO;
    if (e != E_VEHICULO_NO_EXISTE) return e;

    aMayusculas(datos.placa);
    aMayusculas(datos.cedulaPropietario);
    e = guardarVehiculo(datos);
    if (e == E_OK && idNuevo != nullptr) *idNuevo = datos.id;
    return e;
}

CodigoError buscarVehiculosPorPropietario(const char* subcadena, Vehiculo* res, int& cantidad, bool& hayMas) {
    cantidad = 0;
    hayMas = false;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        Vehiculo v;
        e = leerVehiculoPorID(id, v);
        if (e != E_OK) return e;
        if (v.eliminado || !contieneIgnorandoMayusculas(v.nombrePropietario, subcadena)) continue;
        if (cantidad == MAX_RESULTADOS) { hayMas = true; break; }
        res[cantidad++] = v;
    }
    return E_OK;
}

CodigoError listarVehiculos(Vehiculo* res, int& cantidad, bool& hayMas) {
    cantidad = 0;
    hayMas = false;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        Vehiculo v;
        e = leerVehiculoPorID(id, v);
        if (e != E_OK) return e;
        if (v.eliminado) continue;
        if (cantidad == MAX_RESULTADOS) { hayMas = true; break; }
        res[cantidad++] = v;
    }
    return E_OK;
}

CodigoError actualizarVehiculo(int id, Vehiculo datos) {
    Vehiculo v;
    CodigoError e = obtenerVehiculoPorID(id, v);
    if (e != E_OK) return e;
    if (!datosEditablesValidos(datos)) return E_DATOS_INVALIDOS;
    // No se tocan: id, placa, kilometraje ni fechaCreacion
    copiarTexto(v.marca, datos.marca, sizeof v.marca);
    copiarTexto(v.modelo, datos.modelo, sizeof v.modelo);
    v.anio = datos.anio;
    copiarTexto(v.nombrePropietario, datos.nombrePropietario, sizeof v.nombrePropietario);
    copiarTexto(v.cedulaPropietario, datos.cedulaPropietario, sizeof v.cedulaPropietario);
    aMayusculas(v.cedulaPropietario);
    copiarTexto(v.telefonoPropietario, datos.telefonoPropietario, sizeof v.telefonoPropietario);
    return sobrescribirVehiculo(v);
}

CodigoError eliminarVehiculo(int id) {
    Vehiculo v;
    CodigoError e = obtenerVehiculoPorID(id, v);
    if (e != E_OK) return e;
    int ordenes = 0;
    e = contarOrdenesDeVehiculo(id, ordenes);
    if (e != E_OK) return e;
    if (ordenes > 0) return E_VEHICULO_CON_ORDENES;     // en cualquier estado
    return marcarVehiculoEliminado(id);
}

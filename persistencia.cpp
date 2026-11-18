// CAPA DE PERSISTENCIA - Operaciones crudas sobre los archivos binarios de entidades.
// Sin cin/cout y sin reglas de negocio: no filtra eliminados, no valida datos.
//
// Relacion ID <-> indice fisico:  indice = id - 1   (ids 1,2,3... asignados en orden de insercion;
// los registros nunca se mueven ni se borran fisicamente).
//   posicion = sizeof(ArchivoHeader) + (id - 1) * sizeof(Estructura)
//
// Las 5 operaciones se implementan POR ENTIDAD (la repeticion se resuelve en el Proyecto 3).
#include "taller.h"
#include <fstream>
using namespace std;

// =====================================================================
//  VEHICULOS
// =====================================================================
CodigoError guardarVehiculo(Vehiculo& v) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    if (h.proximoID != h.cantidadRegistros + 1) return errorCorrupto(RUTA_VEHICULOS);

    time_t ahora = time(nullptr);
    v.id = h.proximoID;
    v.eliminado = false;
    v.fechaCreacion = ahora;
    v.fechaUltimaModificacion = ahora;

    fstream f(RUTA_VEHICULOS, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_VEHICULOS);
    f.seekp(posicionRegistro(sizeof(Vehiculo), h.cantidadRegistros));   // al final
    f.write(reinterpret_cast<const char*>(&v), sizeof v);
    f.close();
    if (!f) return errorArchivo(RUTA_VEHICULOS);

    // Primero el registro, despues el header (ver analisis de interrupciones en README_V2)
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID++;
    return escribirHeader(RUTA_VEHICULOS, h);
}

CodigoError leerVehiculoPorID(int id, Vehiculo& out, long* posicion) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    if (id < 1 || id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    long pos = posicionRegistro(sizeof(Vehiculo), id - 1);
    ifstream f(RUTA_VEHICULOS, ios::binary);
    if (!f) return errorArchivo(RUTA_VEHICULOS);
    f.seekg(pos);
    f.read(reinterpret_cast<char*>(&out), sizeof out);
    if (f.gcount() != (streamsize)sizeof out) return errorCorrupto(RUTA_VEHICULOS);
    if (posicion != nullptr) *posicion = pos;
    return E_OK;
}

CodigoError sobrescribirVehiculo(Vehiculo& v) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    if (v.id < 1 || v.id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    v.fechaUltimaModificacion = time(nullptr);
    fstream f(RUTA_VEHICULOS, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_VEHICULOS);
    f.seekp(posicionRegistro(sizeof(Vehiculo), v.id - 1));
    f.write(reinterpret_cast<const char*>(&v), sizeof v);
    f.close();
    if (!f) return errorArchivo(RUTA_VEHICULOS);
    return E_OK;
}

CodigoError marcarVehiculoEliminado(int id) {
    Vehiculo v;
    CodigoError e = leerVehiculoPorID(id, v);
    if (e != E_OK) return e;
    if (v.eliminado) return E_ELIMINADO;          // ya estaba eliminado: no se descuenta dos veces
    v.eliminado = true;
    e = sobrescribirVehiculo(v);
    if (e != E_OK) return e;
    ArchivoHeader h;
    e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    h.registrosActivos--;
    return escribirHeader(RUTA_VEHICULOS, h);
}

CodigoError contarVehiculosActivos(int& cantidad) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_VEHICULOS, h);
    if (e != E_OK) return e;
    cantidad = h.registrosActivos;
    return E_OK;
}

// =====================================================================
//  MECANICOS
// =====================================================================
CodigoError guardarMecanico(Mecanico& m) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_MECANICOS, h);
    if (e != E_OK) return e;
    if (h.proximoID != h.cantidadRegistros + 1) return errorCorrupto(RUTA_MECANICOS);

    time_t ahora = time(nullptr);
    m.id = h.proximoID;
    m.eliminado = false;
    m.fechaCreacion = ahora;
    m.fechaUltimaModificacion = ahora;

    fstream f(RUTA_MECANICOS, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_MECANICOS);
    f.seekp(posicionRegistro(sizeof(Mecanico), h.cantidadRegistros));
    f.write(reinterpret_cast<const char*>(&m), sizeof m);
    f.close();
    if (!f) return errorArchivo(RUTA_MECANICOS);

    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID++;
    return escribirHeader(RUTA_MECANICOS, h);
}

CodigoError leerMecanicoPorID(int id, Mecanico& out, long* posicion) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_MECANICOS, h);
    if (e != E_OK) return e;
    if (id < 1 || id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    long pos = posicionRegistro(sizeof(Mecanico), id - 1);
    ifstream f(RUTA_MECANICOS, ios::binary);
    if (!f) return errorArchivo(RUTA_MECANICOS);
    f.seekg(pos);
    f.read(reinterpret_cast<char*>(&out), sizeof out);
    if (f.gcount() != (streamsize)sizeof out) return errorCorrupto(RUTA_MECANICOS);
    if (posicion != nullptr) *posicion = pos;
    return E_OK;
}

CodigoError sobrescribirMecanico(Mecanico& m) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_MECANICOS, h);
    if (e != E_OK) return e;
    if (m.id < 1 || m.id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    m.fechaUltimaModificacion = time(nullptr);
    fstream f(RUTA_MECANICOS, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_MECANICOS);
    f.seekp(posicionRegistro(sizeof(Mecanico), m.id - 1));
    f.write(reinterpret_cast<const char*>(&m), sizeof m);
    f.close();
    if (!f) return errorArchivo(RUTA_MECANICOS);
    return E_OK;
}

CodigoError marcarMecanicoEliminado(int id) {
    Mecanico m;
    CodigoError e = leerMecanicoPorID(id, m);
    if (e != E_OK) return e;
    if (m.eliminado) return E_ELIMINADO;
    m.eliminado = true;
    e = sobrescribirMecanico(m);
    if (e != E_OK) return e;
    ArchivoHeader h;
    e = leerHeader(RUTA_MECANICOS, h);
    if (e != E_OK) return e;
    h.registrosActivos--;
    return escribirHeader(RUTA_MECANICOS, h);
}

CodigoError contarMecanicosActivos(int& cantidad) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_MECANICOS, h);
    if (e != E_OK) return e;
    cantidad = h.registrosActivos;
    return E_OK;
}

// =====================================================================
//  REPUESTOS
// =====================================================================
CodigoError guardarRepuesto(Repuesto& r) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    if (h.proximoID != h.cantidadRegistros + 1) return errorCorrupto(RUTA_REPUESTOS);

    time_t ahora = time(nullptr);
    r.id = h.proximoID;
    r.eliminado = false;
    r.fechaCreacion = ahora;
    r.fechaUltimaModificacion = ahora;

    fstream f(RUTA_REPUESTOS, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_REPUESTOS);
    f.seekp(posicionRegistro(sizeof(Repuesto), h.cantidadRegistros));
    f.write(reinterpret_cast<const char*>(&r), sizeof r);
    f.close();
    if (!f) return errorArchivo(RUTA_REPUESTOS);

    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID++;
    return escribirHeader(RUTA_REPUESTOS, h);
}

CodigoError leerRepuestoPorID(int id, Repuesto& out, long* posicion) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    if (id < 1 || id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    long pos = posicionRegistro(sizeof(Repuesto), id - 1);
    ifstream f(RUTA_REPUESTOS, ios::binary);
    if (!f) return errorArchivo(RUTA_REPUESTOS);
    f.seekg(pos);
    f.read(reinterpret_cast<char*>(&out), sizeof out);
    if (f.gcount() != (streamsize)sizeof out) return errorCorrupto(RUTA_REPUESTOS);
    if (posicion != nullptr) *posicion = pos;
    return E_OK;
}

CodigoError sobrescribirRepuesto(Repuesto& r) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    if (r.id < 1 || r.id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    r.fechaUltimaModificacion = time(nullptr);
    fstream f(RUTA_REPUESTOS, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_REPUESTOS);
    f.seekp(posicionRegistro(sizeof(Repuesto), r.id - 1));
    f.write(reinterpret_cast<const char*>(&r), sizeof r);
    f.close();
    if (!f) return errorArchivo(RUTA_REPUESTOS);
    return E_OK;
}

CodigoError marcarRepuestoEliminado(int id) {
    Repuesto r;
    CodigoError e = leerRepuestoPorID(id, r);
    if (e != E_OK) return e;
    if (r.eliminado) return E_ELIMINADO;
    r.eliminado = true;
    e = sobrescribirRepuesto(r);
    if (e != E_OK) return e;
    ArchivoHeader h;
    e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    h.registrosActivos--;
    return escribirHeader(RUTA_REPUESTOS, h);
}

CodigoError contarRepuestosActivos(int& cantidad) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_REPUESTOS, h);
    if (e != E_OK) return e;
    cantidad = h.registrosActivos;
    return E_OK;
}

// =====================================================================
//  ORDENES (el detalle viaja dentro del registro: ver OrdenServicio.lineas)
// =====================================================================
CodigoError guardarOrden(OrdenServicio& o) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    if (h.proximoID != h.cantidadRegistros + 1) return errorCorrupto(RUTA_ORDENES);

    time_t ahora = time(nullptr);
    o.id = h.proximoID;
    o.eliminado = false;
    o.fechaCreacion = ahora;
    o.fechaUltimaModificacion = ahora;

    fstream f(RUTA_ORDENES, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_ORDENES);
    f.seekp(posicionRegistro(sizeof(OrdenServicio), h.cantidadRegistros));
    f.write(reinterpret_cast<const char*>(&o), sizeof o);
    f.close();
    if (!f) return errorArchivo(RUTA_ORDENES);

    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID++;
    return escribirHeader(RUTA_ORDENES, h);
}

CodigoError leerOrdenPorID(int id, OrdenServicio& out, long* posicion) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    if (id < 1 || id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    long pos = posicionRegistro(sizeof(OrdenServicio), id - 1);
    ifstream f(RUTA_ORDENES, ios::binary);
    if (!f) return errorArchivo(RUTA_ORDENES);
    f.seekg(pos);
    f.read(reinterpret_cast<char*>(&out), sizeof out);
    if (f.gcount() != (streamsize)sizeof out) return errorCorrupto(RUTA_ORDENES);
    if (posicion != nullptr) *posicion = pos;
    return E_OK;
}

CodigoError sobrescribirOrden(OrdenServicio& o) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    if (o.id < 1 || o.id > h.cantidadRegistros) return E_ID_NO_EXISTE;

    o.fechaUltimaModificacion = time(nullptr);
    fstream f(RUTA_ORDENES, ios::in | ios::out | ios::binary);
    if (!f) return errorArchivo(RUTA_ORDENES);
    f.seekp(posicionRegistro(sizeof(OrdenServicio), o.id - 1));
    f.write(reinterpret_cast<const char*>(&o), sizeof o);
    f.close();
    if (!f) return errorArchivo(RUTA_ORDENES);
    return E_OK;
}

CodigoError marcarOrdenEliminada(int id) {
    OrdenServicio o;
    CodigoError e = leerOrdenPorID(id, o);
    if (e != E_OK) return e;
    if (o.eliminado) return E_ELIMINADO;
    o.eliminado = true;
    e = sobrescribirOrden(o);
    if (e != E_OK) return e;
    ArchivoHeader h;
    e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    h.registrosActivos--;
    return escribirHeader(RUTA_ORDENES, h);
}

CodigoError contarOrdenesActivas(int& cantidad) {
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    cantidad = h.registrosActivos;
    return E_OK;
}

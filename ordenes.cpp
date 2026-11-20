// CAPA DE LOGICA - Ordenes de servicio. SIN cin/cout.
// El detalle vive DENTRO del registro de la orden (OrdenServicio.lineas / numLineas).
//
// Operaciones compuestas: TODAS las validaciones se hacen antes de la primera escritura.
// Orden de escritura elegido (principio: el stock nunca queda MAYOR que el real):
//   * lo que DESCUENTA stock se escribe primero   (agregar: stock -> orden)
//   * lo que DEVUELVE stock se escribe despues     (quitar / anular: orden -> stock)
// Asi una interrupcion deja unidades "perdidas" (recuperables con una entrada de mercancia)
// y nunca unidades "inventadas" que se podrian vender sin existir. Ver README_V2.md.
#include "taller.h"
#include <cstring>

static bool estaAbierta(const OrdenServicio& o) { return strcmp(o.estado, "ABIERTA") == 0; }

CodigoError obtenerOrdenPorID(int id, OrdenServicio& out, long* posicion) {
    return leerOrdenPorID(id, out, posicion);
}

float calcularSubtotalRepuestos(const OrdenServicio& o) {
    float suma = 0;
    for (int i = 0; i < o.numLineas; i++)
        suma += o.lineas[i].cantidad * o.lineas[i].precioUnitario;
    return suma;
}

float calcularTotalOrden(const OrdenServicio& o) {
    return o.manoObra + calcularSubtotalRepuestos(o);
}

// ---------- Consultas (un registro en memoria a la vez) ----------
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
        if (o.idMecanico == idMecanico && estaAbierta(o)) cantidad++;
    }
    return E_OK;
}

CodigoError ordenAbiertaDeVehiculo(int idVehiculo, int& idOrden) {
    idOrden = 0;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) return e;
        if (o.idVehiculo == idVehiculo && estaAbierta(o)) { idOrden = o.id; return E_OK; }
    }
    return E_OK;
}

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

// Listados con tope MAX_RESULTADOS. 'estado' == nullptr no filtra; idVehiculo <= 0 no filtra.
static CodigoError listarFiltrado(const char* estado, int idVehiculo,
                                  OrdenServicio* res, int& cantidad, bool& hayMas) {
    cantidad = 0;
    hayMas = false;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) return e;
        if (estado != nullptr && !igualesIgnorandoMayusculas(o.estado, estado)) continue;
        if (idVehiculo > 0 && o.idVehiculo != idVehiculo) continue;
        if (cantidad == MAX_RESULTADOS) { hayMas = true; break; }
        res[cantidad++] = o;
    }
    return E_OK;
}

CodigoError listarOrdenes(OrdenServicio* res, int& cantidad, bool& hayMas) {
    return listarFiltrado(nullptr, 0, res, cantidad, hayMas);
}
CodigoError listarOrdenesPorEstado(const char* estado, OrdenServicio* res, int& cantidad, bool& hayMas) {
    return listarFiltrado(estado, 0, res, cantidad, hayMas);
}
CodigoError listarOrdenesPorVehiculo(int idVehiculo, OrdenServicio* res, int& cantidad, bool& hayMas) {
    return listarFiltrado(nullptr, idVehiculo, res, cantidad, hayMas);
}

// ---------- Operaciones compuestas ----------
// Lee: vehiculos, mecanicos, ordenes.  Escribe: ordenes, vehiculos (kilometraje)
CodigoError abrirOrden(int idVehiculo, int idMecanico, int kilometrajeIngreso,
                       const char* descripcionFalla, int* idNuevo) {
    // --- validaciones (ninguna escritura todavia) ---
    Vehiculo v;
    CodigoError e = obtenerVehiculoPorID(idVehiculo, v);
    if (e == E_ID_NO_EXISTE) return E_VEHICULO_NO_EXISTE;
    if (e == E_ELIMINADO) return E_VEHICULO_INACTIVO;
    if (e != E_OK) return e;

    Mecanico m;
    e = obtenerMecanicoPorID(idMecanico, m);
    if (e == E_ID_NO_EXISTE) return E_MECANICO_NO_EXISTE;
    if (e == E_ELIMINADO) return E_MECANICO_INACTIVO;
    if (e != E_OK) return e;

    if (!textoNoVacio(descripcionFalla)) return E_DATOS_INVALIDOS;

    int abierta = 0;
    e = ordenAbiertaDeVehiculo(idVehiculo, abierta);
    if (e != E_OK) return e;
    if (abierta != 0) return E_VEHICULO_CON_ORDEN_ABIERTA;

    int abiertasMec = 0;
    e = contarOrdenesAbiertasMecanico(idMecanico, abiertasMec);
    if (e != E_OK) return e;
    if (abiertasMec >= 3) return E_MECANICO_LLENO;

    if (kilometrajeIngreso < v.kilometraje) return E_KM_MENOR;

    // --- escrituras: primero la orden, despues el kilometraje del vehiculo ---
    OrdenServicio o;
    memset(&o, 0, sizeof o);
    o.idVehiculo = idVehiculo;
    o.idMecanico = idMecanico;
    fechaActual(o.fechaApertura);
    o.fechaCierre[0] = '\0';
    copiarTexto(o.estado, "ABIERTA", sizeof o.estado);
    copiarTexto(o.descripcionFalla, descripcionFalla, sizeof o.descripcionFalla);
    o.kilometrajeIngreso = kilometrajeIngreso;
    o.manoObra = 0;
    o.numLineas = 0;
    e = guardarOrden(o);
    if (e != E_OK) return e;
    if (idNuevo != nullptr) *idNuevo = o.id;

    v.kilometraje = kilometrajeIngreso;
    return sobrescribirVehiculo(v);
}

// Lee: ordenes, repuestos.  Escribe: repuestos (stock) y luego ordenes (detalle)
CodigoError agregarRepuestoAOrden(int idOrden, int idRepuesto, int cantidad) {
    OrdenServicio o;
    CodigoError e = leerOrdenPorID(idOrden, o);
    if (e == E_ID_NO_EXISTE) return E_ORDEN_NO_EXISTE;
    if (e != E_OK) return e;

    Repuesto r;
    e = obtenerRepuestoPorID(idRepuesto, r);
    if (e == E_ID_NO_EXISTE || e == E_ELIMINADO) return E_REPUESTO_NO_EXISTE;
    if (e != E_OK) return e;

    if (!estaAbierta(o)) return E_ORDEN_NO_ABIERTA;
    if (cantidad <= 0) return E_CANTIDAD_INVALIDA;
    if (r.stock < cantidad) return E_STOCK_INSUFICIENTE;

    int pos = -1;
    for (int i = 0; i < o.numLineas; i++)
        if (o.lineas[i].idRepuesto == idRepuesto) { pos = i; break; }
    if (pos == -1 && o.numLineas >= MAX_LINEAS_ORDEN) return E_LINEAS_LLENAS;

    // --- escrituras ---
    r.stock -= cantidad;
    e = sobrescribirRepuesto(r);                 // 1) descuenta el stock
    if (e != E_OK) return e;
    if (pos != -1) {
        o.lineas[pos].cantidad += cantidad;      // acumula; conserva el precio historico
    } else {
        o.lineas[o.numLineas].idRepuesto = idRepuesto;
        o.lineas[o.numLineas].cantidad = cantidad;
        o.lineas[o.numLineas].precioUnitario = r.precio;   // precio vigente AHORA
        o.numLineas++;
    }
    return sobrescribirOrden(o);                 // 2) registra la linea
}

// Lee: ordenes.  Escribe: ordenes (quita la linea) y luego repuestos (devuelve stock)
CodigoError quitarRepuestoDeOrden(int idOrden, int idRepuesto) {
    OrdenServicio o;
    CodigoError e = leerOrdenPorID(idOrden, o);
    if (e == E_ID_NO_EXISTE) return E_ORDEN_NO_EXISTE;
    if (e != E_OK) return e;

    Repuesto r;
    e = leerRepuestoPorID(idRepuesto, r);        // crudo: un repuesto usado en ordenes no se elimina
    if (e == E_ID_NO_EXISTE) return E_REPUESTO_NO_EXISTE;
    if (e != E_OK) return e;

    if (!estaAbierta(o)) return E_ORDEN_NO_ABIERTA;
    int pos = -1;
    for (int i = 0; i < o.numLineas; i++)
        if (o.lineas[i].idRepuesto == idRepuesto) { pos = i; break; }
    if (pos == -1) return E_REPUESTO_NO_EN_ORDEN;

    // --- escrituras ---
    int devuelto = o.lineas[pos].cantidad;
    for (int i = pos; i < o.numLineas - 1; i++) o.lineas[i] = o.lineas[i + 1];   // compacta
    o.numLineas--;
    memset(&o.lineas[o.numLineas], 0, sizeof(DetalleOrden));
    e = sobrescribirOrden(o);                    // 1) quita la linea
    if (e != E_OK) return e;
    r.stock += devuelto;
    return sobrescribirRepuesto(r);              // 2) devuelve el stock
}

// Lee: ordenes.  Escribe: ordenes
CodigoError cerrarOrden(int idOrden, float manoObra) {
    OrdenServicio o;
    CodigoError e = leerOrdenPorID(idOrden, o);
    if (e == E_ID_NO_EXISTE) return E_ORDEN_NO_EXISTE;
    if (e != E_OK) return e;
    if (!estaAbierta(o)) return E_ORDEN_NO_ABIERTA;
    if (manoObra < 0) return E_MANO_OBRA_INVALIDA;
    if (o.numLineas == 0 && manoObra <= 0) return E_ORDEN_SIN_CONTENIDO;

    o.manoObra = manoObra;
    copiarTexto(o.estado, "CERRADA", sizeof o.estado);
    fechaActual(o.fechaCierre);
    return sobrescribirOrden(o);
}

// Lee: ordenes.  Escribe: ordenes (ANULADA) y luego repuestos (stock de cada linea)
CodigoError anularOrden(int idOrden) {
    OrdenServicio o;
    CodigoError e = leerOrdenPorID(idOrden, o);
    if (e == E_ID_NO_EXISTE) return E_ORDEN_NO_EXISTE;
    if (e != E_OK) return e;
    if (strcmp(o.estado, "ABIERTA") != 0 && strcmp(o.estado, "CERRADA") != 0)
        return E_ORDEN_NO_ANULABLE;

    // Validacion previa: todos los repuestos de las lineas deben existir (antes de escribir)
    for (int i = 0; i < o.numLineas; i++) {
        Repuesto r;
        e = leerRepuestoPorID(o.lineas[i].idRepuesto, r);
        if (e == E_ID_NO_EXISTE) return E_REPUESTO_NO_EXISTE;
        if (e != E_OK) return e;
    }

    // --- escrituras ---
    copiarTexto(o.estado, "ANULADA", sizeof o.estado);     // las lineas se conservan (historial)
    e = sobrescribirOrden(o);                    // 1) marca la orden
    if (e != E_OK) return e;
    for (int i = 0; i < o.numLineas; i++) {      // 2) devuelve el stock linea por linea
        Repuesto r;
        e = leerRepuestoPorID(o.lineas[i].idRepuesto, r);
        if (e != E_OK) return e;
        r.stock += o.lineas[i].cantidad;
        e = sobrescribirRepuesto(r);
        if (e != E_OK) return e;
    }
    return E_OK;
}

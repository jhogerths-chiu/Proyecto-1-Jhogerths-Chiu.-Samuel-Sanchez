// CAPA DE LOGICA - Ordenes de servicio. SIN cin/cout.
#include "taller.h"
#include <cstring>

OrdenServicio* buscarOrdenPorID(SistemaTaller* s, int id) {
    for (int i = 0; i < s->numOrdenes; i++)
        if (s->ordenes[i].id == id) return &s->ordenes[i];
    return nullptr;
}

OrdenServicio* ordenAbiertaDeVehiculo(SistemaTaller* s, int idVehiculo) {
    for (int i = 0; i < s->numOrdenes; i++)
        if (s->ordenes[i].idVehiculo == idVehiculo &&
            strcmp(s->ordenes[i].estado, "ABIERTA") == 0) return &s->ordenes[i];
    return nullptr;
}

int contarOrdenesDeVehiculo(SistemaTaller* s, int idVehiculo) {
    int n = 0;
    for (int i = 0; i < s->numOrdenes; i++)
        if (s->ordenes[i].idVehiculo == idVehiculo) n++;
    return n;
}

OrdenServicio* abrirOrden(SistemaTaller* s, int idVehiculo, int idMecanico,
                          int kilometrajeIngreso, const char* descripcionFalla,
                          CodigoError* error) {
    CodigoError tmp;
    if (error == nullptr) error = &tmp;

    Vehiculo* v = buscarVehiculoPorID(s, idVehiculo);
    if (v == nullptr) { *error = E_VEHICULO_NO_EXISTE; return nullptr; }
    if (buscarMecanicoPorID(s, idMecanico) == nullptr) { *error = E_MECANICO_NO_EXISTE; return nullptr; }
    if (!textoNoVacio(descripcionFalla)) { *error = E_DATOS_INVALIDOS; return nullptr; }
    if (ordenAbiertaDeVehiculo(s, idVehiculo) != nullptr) { *error = E_VEHICULO_CON_ORDEN_ABIERTA; return nullptr; }
    if (contarOrdenesAbiertasMecanico(s, idMecanico) >= 3) { *error = E_MECANICO_LLENO; return nullptr; }
    if (kilometrajeIngreso < v->kilometraje) { *error = E_KM_MENOR; return nullptr; }

    // Redimensionar el arreglo de ordenes. Al copiar cada OrdenServicio se copia el
    // PUNTERO 'detalles' (no el arreglo): el arreglo de detalles no se mueve ni se pierde.
    duplicarSiLleno(s->ordenes, s->numOrdenes, s->capacidadOrdenes);

    OrdenServicio* o = &s->ordenes[s->numOrdenes];
    o->id = s->siguienteIdOrden++;
    o->idVehiculo = idVehiculo;
    o->idMecanico = idMecanico;
    fechaActual(o->fechaApertura);
    o->fechaCierre[0] = '\0';
    copiarTexto(o->estado, "ABIERTA", sizeof o->estado);
    copiarTexto(o->descripcionFalla, descripcionFalla, sizeof o->descripcionFalla);
    o->kilometrajeIngreso = kilometrajeIngreso;
    o->manoObra = 0;
    o->capacidadDetalles = 2;
    o->numDetalles = 0;
    o->detalles = new DetalleOrden[o->capacidadDetalles];
    s->numOrdenes++;

    v->kilometraje = kilometrajeIngreso;   // se actualiza el km del vehiculo
    *error = E_OK;
    return o;
}

CodigoError agregarRepuestoAOrden(SistemaTaller* s, int idOrden, int idRepuesto, int cantidad) {
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) return E_ORDEN_NO_EXISTE;
    Repuesto* r = buscarRepuestoPorID(s, idRepuesto);
    if (r == nullptr) return E_REPUESTO_NO_EXISTE;
    if (strcmp(o->estado, "ABIERTA") != 0) return E_ORDEN_NO_ABIERTA;
    if (cantidad <= 0) return E_CANTIDAD_INVALIDA;
    if (r->stock < cantidad) return E_STOCK_INSUFICIENTE;

    // Si el repuesto ya esta en la orden, se acumula en la linea existente
    for (int i = 0; i < o->numDetalles; i++) {
        if (o->detalles[i].idRepuesto == idRepuesto) {
            o->detalles[i].cantidad += cantidad;
            r->stock -= cantidad;
            return E_OK;
        }
    }
    duplicarSiLleno(o->detalles, o->numDetalles, o->capacidadDetalles);
    DetalleOrden d;
    d.idRepuesto = idRepuesto;
    d.cantidad = cantidad;
    d.precioUnitario = r->precio;          // precio vigente AHORA
    o->detalles[o->numDetalles++] = d;
    r->stock -= cantidad;
    return E_OK;
}

CodigoError quitarRepuestoDeOrden(SistemaTaller* s, int idOrden, int idRepuesto) {
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) return E_ORDEN_NO_EXISTE;
    Repuesto* r = buscarRepuestoPorID(s, idRepuesto);
    if (r == nullptr) return E_REPUESTO_NO_EXISTE;
    if (strcmp(o->estado, "ABIERTA") != 0) return E_ORDEN_NO_ABIERTA;

    int pos = -1;
    for (int i = 0; i < o->numDetalles; i++)
        if (o->detalles[i].idRepuesto == idRepuesto) { pos = i; break; }
    if (pos == -1) return E_REPUESTO_NO_EN_ORDEN;

    r->stock += o->detalles[pos].cantidad;     // devuelve TODA la linea al inventario
    for (int i = pos; i < o->numDetalles - 1; i++)
        o->detalles[i] = o->detalles[i + 1];
    o->numDetalles--;
    return E_OK;
}

CodigoError cerrarOrden(SistemaTaller* s, int idOrden, float manoObra) {
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) return E_ORDEN_NO_EXISTE;
    if (strcmp(o->estado, "ABIERTA") != 0) return E_ORDEN_NO_ABIERTA;
    if (manoObra < 0) return E_MANO_OBRA_INVALIDA;
    if (o->numDetalles == 0 && manoObra <= 0) return E_ORDEN_SIN_CONTENIDO;
    o->manoObra = manoObra;
    copiarTexto(o->estado, "CERRADA", sizeof o->estado);
    fechaActual(o->fechaCierre);
    return E_OK;
}

CodigoError anularOrden(SistemaTaller* s, int idOrden) {
    OrdenServicio* o = buscarOrdenPorID(s, idOrden);
    if (o == nullptr) return E_ORDEN_NO_EXISTE;
    if (strcmp(o->estado, "ABIERTA") != 0 && strcmp(o->estado, "CERRADA") != 0)
        return E_ORDEN_NO_ANULABLE;
    for (int i = 0; i < o->numDetalles; i++) {
        Repuesto* r = buscarRepuestoPorID(s, o->detalles[i].idRepuesto);
        if (r != nullptr) r->stock += o->detalles[i].cantidad;
    }
    // Las lineas se conservan para el historial; solo cambia el estado
    copiarTexto(o->estado, "ANULADA", sizeof o->estado);
    return E_OK;
}

float calcularSubtotalRepuestos(const OrdenServicio* o) {
    float suma = 0;
    for (int i = 0; i < o->numDetalles; i++)
        suma += o->detalles[i].cantidad * o->detalles[i].precioUnitario;
    return suma;
}

float calcularTotalOrden(const OrdenServicio* o) {
    return o->manoObra + calcularSubtotalRepuestos(o);
}

OrdenServicio** listarOrdenes(SistemaTaller* s, int* cantidad) {
    *cantidad = s->numOrdenes;
    if (s->numOrdenes == 0) return nullptr;
    OrdenServicio** res = new OrdenServicio*[s->numOrdenes];
    for (int i = 0; i < s->numOrdenes; i++) res[i] = &s->ordenes[i];
    return res;
}

OrdenServicio** listarOrdenesPorEstado(SistemaTaller* s, const char* estado, int* cantidad) {
    int n = 0;
    for (int i = 0; i < s->numOrdenes; i++)
        if (igualesIgnorandoMayusculas(s->ordenes[i].estado, estado)) n++;
    *cantidad = n;
    if (n == 0) return nullptr;
    OrdenServicio** res = new OrdenServicio*[n];
    int k = 0;
    for (int i = 0; i < s->numOrdenes; i++)
        if (igualesIgnorandoMayusculas(s->ordenes[i].estado, estado)) res[k++] = &s->ordenes[i];
    return res;
}

OrdenServicio** listarOrdenesPorVehiculo(SistemaTaller* s, int idVehiculo, int* cantidad) {
    int n = contarOrdenesDeVehiculo(s, idVehiculo);
    *cantidad = n;
    if (n == 0) return nullptr;
    OrdenServicio** res = new OrdenServicio*[n];
    int k = 0;
    for (int i = 0; i < s->numOrdenes; i++)
        if (s->ordenes[i].idVehiculo == idVehiculo) res[k++] = &s->ordenes[i];
    return res;
}

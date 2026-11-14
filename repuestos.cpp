// CAPA DE LOGICA - Repuestos (inventario). SIN cin/cout.
#include "taller.h"
#include <cstring>

Repuesto* buscarRepuestoPorID(SistemaTaller* s, int id) {
    for (int i = 0; i < s->numRepuestos; i++)
        if (s->repuestos[i].id == id) return &s->repuestos[i];
    return nullptr;
}

Repuesto* buscarRepuestoPorCodigo(SistemaTaller* s, const char* codigo) {
    for (int i = 0; i < s->numRepuestos; i++)
        if (igualesIgnorandoMayusculas(s->repuestos[i].codigo, codigo)) return &s->repuestos[i];
    return nullptr;
}

Repuesto* agregarRepuesto(SistemaTaller* s, Repuesto datos, CodigoError* error) {
    CodigoError tmp;
    if (error == nullptr) error = &tmp;

    if (!textoNoVacio(datos.codigo) || !textoNoVacio(datos.nombre) || !textoNoVacio(datos.marca) ||
        datos.precio <= 0 || datos.stock < 0 || datos.stockMinimo < 0) {
        *error = E_DATOS_INVALIDOS;
        return nullptr;
    }
    if (buscarRepuestoPorCodigo(s, datos.codigo) != nullptr) {
        *error = E_DUPLICADO;
        return nullptr;
    }
    duplicarSiLleno(s->repuestos, s->numRepuestos, s->capacidadRepuestos);

    Repuesto* nuevo = &s->repuestos[s->numRepuestos];
    *nuevo = datos;
    nuevo->id = s->siguienteIdRepuesto++;
    aMayusculas(nuevo->codigo);
    fechaActual(nuevo->fechaRegistro);
    s->numRepuestos++;
    *error = E_OK;
    return nuevo;
}

Repuesto** buscarRepuestosPorTexto(SistemaTaller* s, const char* subcadena, int* cantidad) {
    int n = 0;
    for (int i = 0; i < s->numRepuestos; i++)
        if (contieneIgnorandoMayusculas(s->repuestos[i].codigo, subcadena) ||
            contieneIgnorandoMayusculas(s->repuestos[i].nombre, subcadena)) n++;
    *cantidad = n;
    if (n == 0) return nullptr;
    Repuesto** res = new Repuesto*[n];
    int k = 0;
    for (int i = 0; i < s->numRepuestos; i++)
        if (contieneIgnorandoMayusculas(s->repuestos[i].codigo, subcadena) ||
            contieneIgnorandoMayusculas(s->repuestos[i].nombre, subcadena))
            res[k++] = &s->repuestos[i];
    return res;
}

Repuesto** listarRepuestos(SistemaTaller* s, int* cantidad) {
    *cantidad = s->numRepuestos;
    if (s->numRepuestos == 0) return nullptr;
    Repuesto** res = new Repuesto*[s->numRepuestos];
    for (int i = 0; i < s->numRepuestos; i++) res[i] = &s->repuestos[i];
    return res;
}

CodigoError registrarEntradaMercancia(SistemaTaller* s, int idRepuesto, int cantidad) {
    Repuesto* r = buscarRepuestoPorID(s, idRepuesto);
    if (r == nullptr) return E_REPUESTO_NO_EXISTE;
    if (cantidad <= 0) return E_CANTIDAD_INVALIDA;
    r->stock += cantidad;
    return E_OK;
}

CodigoError cambiarPrecioRepuesto(SistemaTaller* s, int idRepuesto, float nuevoPrecio) {
    Repuesto* r = buscarRepuestoPorID(s, idRepuesto);
    if (r == nullptr) return E_REPUESTO_NO_EXISTE;
    if (nuevoPrecio <= 0) return E_PRECIO_INVALIDO;
    r->precio = nuevoPrecio;   // las ordenes existentes conservan su precioUnitario
    return E_OK;
}

bool repuestoUsadoEnOrdenes(SistemaTaller* s, int idRepuesto) {
    for (int i = 0; i < s->numOrdenes; i++)
        for (int j = 0; j < s->ordenes[i].numDetalles; j++)
            if (s->ordenes[i].detalles[j].idRepuesto == idRepuesto) return true;
    return false;
}

CodigoError eliminarRepuesto(SistemaTaller* s, int idRepuesto) {
    int pos = -1;
    for (int i = 0; i < s->numRepuestos; i++)
        if (s->repuestos[i].id == idRepuesto) { pos = i; break; }
    if (pos == -1) return E_REPUESTO_NO_EXISTE;
    if (repuestoUsadoEnOrdenes(s, idRepuesto)) return E_REPUESTO_EN_USO;
    for (int i = pos; i < s->numRepuestos - 1; i++)
        s->repuestos[i] = s->repuestos[i + 1];
    s->numRepuestos--;
    return E_OK;
}

bool estaBajoStock(const Repuesto* r) {
    return r->stock <= r->stockMinimo;
}

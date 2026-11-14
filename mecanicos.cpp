// CAPA DE LOGICA - Mecanicos. SIN cin/cout.
#include "taller.h"
#include <cstring>

Mecanico* buscarMecanicoPorID(SistemaTaller* s, int id) {
    for (int i = 0; i < s->numMecanicos; i++)
        if (s->mecanicos[i].id == id) return &s->mecanicos[i];
    return nullptr;
}

Mecanico* agregarMecanico(SistemaTaller* s, Mecanico datos, CodigoError* error) {
    CodigoError tmp;
    if (error == nullptr) error = &tmp;

    if (!textoNoVacio(datos.nombre) || !cedulaValida(datos.cedula) ||
        !especialidadValida(datos.especialidad) || !fechaValida(datos.fechaIngreso)) {
        *error = E_DATOS_INVALIDOS;
        return nullptr;
    }
    for (int i = 0; i < s->numMecanicos; i++) {
        if (igualesIgnorandoMayusculas(s->mecanicos[i].cedula, datos.cedula)) {
            *error = E_DUPLICADO;
            return nullptr;
        }
    }
    duplicarSiLleno(s->mecanicos, s->numMecanicos, s->capacidadMecanicos);

    Mecanico* nuevo = &s->mecanicos[s->numMecanicos];
    *nuevo = datos;
    nuevo->id = s->siguienteIdMecanico++;
    aMayusculas(nuevo->cedula);
    aMayusculas(nuevo->especialidad);
    s->numMecanicos++;
    *error = E_OK;
    return nuevo;
}

Mecanico** listarMecanicos(SistemaTaller* s, int* cantidad) {
    *cantidad = s->numMecanicos;
    if (s->numMecanicos == 0) return nullptr;
    Mecanico** res = new Mecanico*[s->numMecanicos];
    for (int i = 0; i < s->numMecanicos; i++) res[i] = &s->mecanicos[i];
    return res;
}

int contarOrdenesAbiertasMecanico(SistemaTaller* s, int idMecanico) {
    int n = 0;
    for (int i = 0; i < s->numOrdenes; i++)
        if (s->ordenes[i].idMecanico == idMecanico &&
            strcmp(s->ordenes[i].estado, "ABIERTA") == 0) n++;
    return n;
}

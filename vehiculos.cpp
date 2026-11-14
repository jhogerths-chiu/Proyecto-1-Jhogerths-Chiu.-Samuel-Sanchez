// CAPA DE LOGICA - Vehiculos. SIN cin/cout.
#include "taller.h"
#include <cstring>

static bool datosEditablesValidos(const Vehiculo& d) {
    return textoNoVacio(d.marca) && textoNoVacio(d.modelo) &&
           textoNoVacio(d.nombrePropietario) &&
           cedulaValida(d.cedulaPropietario) &&
           textoNoVacio(d.telefonoPropietario) &&
           d.anio >= 1950 && d.anio <= 2027;
}

Vehiculo* buscarVehiculoPorID(SistemaTaller* s, int id) {
    for (int i = 0; i < s->numVehiculos; i++)
        if (s->vehiculos[i].id == id) return &s->vehiculos[i];
    return nullptr;
}

Vehiculo* buscarVehiculoPorPlaca(SistemaTaller* s, const char* placa) {
    for (int i = 0; i < s->numVehiculos; i++)
        if (igualesIgnorandoMayusculas(s->vehiculos[i].placa, placa)) return &s->vehiculos[i];
    return nullptr;
}

Vehiculo* agregarVehiculo(SistemaTaller* s, Vehiculo datos) {
    if (!placaValida(datos.placa)) return nullptr;
    if (!datosEditablesValidos(datos) || datos.kilometraje < 0) return nullptr;
    if (buscarVehiculoPorPlaca(s, datos.placa) != nullptr) return nullptr;

    // Redimensionamiento: si num == capacidad, se duplica la capacidad
    duplicarSiLleno(s->vehiculos, s->numVehiculos, s->capacidadVehiculos);

    Vehiculo* nuevo = &s->vehiculos[s->numVehiculos];
    *nuevo = datos;
    nuevo->id = s->siguienteIdVehiculo++;
    aMayusculas(nuevo->placa);
    aMayusculas(nuevo->cedulaPropietario);
    fechaActual(nuevo->fechaRegistro);
    s->numVehiculos++;
    return nuevo;
}

Vehiculo** buscarVehiculosPorPropietario(SistemaTaller* s, const char* subcadena, int* cantidad) {
    int n = 0;
    for (int i = 0; i < s->numVehiculos; i++)
        if (contieneIgnorandoMayusculas(s->vehiculos[i].nombrePropietario, subcadena)) n++;
    *cantidad = n;
    if (n == 0) return nullptr;
    Vehiculo** res = new Vehiculo*[n];
    int k = 0;
    for (int i = 0; i < s->numVehiculos; i++)
        if (contieneIgnorandoMayusculas(s->vehiculos[i].nombrePropietario, subcadena))
            res[k++] = &s->vehiculos[i];
    return res;
}

Vehiculo** listarVehiculos(SistemaTaller* s, int* cantidad) {
    *cantidad = s->numVehiculos;
    if (s->numVehiculos == 0) return nullptr;
    Vehiculo** res = new Vehiculo*[s->numVehiculos];
    for (int i = 0; i < s->numVehiculos; i++) res[i] = &s->vehiculos[i];
    return res;
}

bool actualizarVehiculo(SistemaTaller* s, int id, Vehiculo datos) {
    Vehiculo* v = buscarVehiculoPorID(s, id);
    if (v == nullptr) return false;
    if (!datosEditablesValidos(datos)) return false;
    // No se tocan: id, placa, kilometraje ni fechaRegistro
    copiarTexto(v->marca, datos.marca, sizeof v->marca);
    copiarTexto(v->modelo, datos.modelo, sizeof v->modelo);
    v->anio = datos.anio;
    copiarTexto(v->nombrePropietario, datos.nombrePropietario, sizeof v->nombrePropietario);
    copiarTexto(v->cedulaPropietario, datos.cedulaPropietario, sizeof v->cedulaPropietario);
    aMayusculas(v->cedulaPropietario);
    copiarTexto(v->telefonoPropietario, datos.telefonoPropietario, sizeof v->telefonoPropietario);
    return true;
}

bool eliminarVehiculo(SistemaTaller* s, int id) {
    int pos = -1;
    for (int i = 0; i < s->numVehiculos; i++)
        if (s->vehiculos[i].id == id) { pos = i; break; }
    if (pos == -1) return false;
    if (contarOrdenesDeVehiculo(s, id) > 0) return false;
    for (int i = pos; i < s->numVehiculos - 1; i++)
        s->vehiculos[i] = s->vehiculos[i + 1];
    s->numVehiculos--;
    return true;
}

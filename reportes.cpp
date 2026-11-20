// CAPA DE LOGICA - Datos de los reportes. SIN cin/cout.
// Se leen los archivos con UN solo registro de orden en memoria a la vez. Solo las ordenes
// CERRADAS suman en los totales (las ABIERTAS y ANULADAS no).
#include "taller.h"
#include <cstring>

CodigoError totalFacturadoVehiculo(int idVehiculo, float& total) {
    total = 0;
    ArchivoHeader h;
    CodigoError e = leerHeader(RUTA_ORDENES, h);
    if (e != E_OK) return e;
    for (int id = 1; id <= h.cantidadRegistros; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) return e;
        if (o.idVehiculo == idVehiculo && strcmp(o.estado, "CERRADA") == 0)
            total += calcularTotalOrden(o);
    }
    return E_OK;
}

// Acumula unidades por repuesto (arreglo de contadores, NO se cargan las ordenes) y
// selecciona los TOP_REPUESTOS mayores.
CodigoError repuestosMasUtilizados(UsoRepuesto* top, int& cantidad) {
    cantidad = 0;
    ArchivoHeader hr;
    CodigoError e = leerHeader(RUTA_REPUESTOS, hr);
    if (e != E_OK) return e;
    ArchivoHeader ho;
    e = leerHeader(RUTA_ORDENES, ho);
    if (e != E_OK) return e;
    int n = hr.cantidadRegistros;
    if (n <= 0) return E_OK;

    int* unidades = new int[n + 1];
    for (int i = 0; i <= n; i++) unidades[i] = 0;

    for (int id = 1; id <= ho.cantidadRegistros; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) { delete[] unidades; return e; }
        if (strcmp(o.estado, "CERRADA") != 0) continue;
        for (int j = 0; j < o.numLineas; j++) {
            int idRep = o.lineas[j].idRepuesto;
            if (idRep >= 1 && idRep <= n) unidades[idRep] += o.lineas[j].cantidad;
        }
    }
    for (int k = 0; k < TOP_REPUESTOS; k++) {
        int mejor = 0;
        for (int i = 1; i <= n; i++)
            if (unidades[i] > 0 && (mejor == 0 || unidades[i] > unidades[mejor])) mejor = i;
        if (mejor == 0) break;
        top[cantidad].idRepuesto = mejor;
        top[cantidad].unidades = unidades[mejor];
        cantidad++;
        unidades[mejor] = 0;     // ya elegido
    }
    delete[] unidades;
    return E_OK;
}

// Una fila por mecanico (activos y dados de baja). La fila i corresponde al mecanico ID i + 1.
CodigoError facturacionPorMecanico(FacturacionMecanico* filas, int& cantidad, bool& hayMas) {
    cantidad = 0;
    hayMas = false;
    ArchivoHeader hm;
    CodigoError e = leerHeader(RUTA_MECANICOS, hm);
    if (e != E_OK) return e;
    ArchivoHeader ho;
    e = leerHeader(RUTA_ORDENES, ho);
    if (e != E_OK) return e;

    int n = hm.cantidadRegistros;
    if (n > MAX_RESULTADOS) { n = MAX_RESULTADOS; hayMas = true; }
    for (int i = 0; i < n; i++) {
        filas[i].idMecanico = i + 1;
        filas[i].ordenesCerradas = 0;
        filas[i].manoObra = 0;
        filas[i].repuestos = 0;
    }
    cantidad = n;

    for (int id = 1; id <= ho.cantidadRegistros; id++) {
        OrdenServicio o;
        e = leerOrdenPorID(id, o);
        if (e != E_OK) return e;
        if (strcmp(o.estado, "CERRADA") != 0) continue;
        if (o.idMecanico < 1 || o.idMecanico > n) continue;
        FacturacionMecanico& f = filas[o.idMecanico - 1];
        f.ordenesCerradas++;
        f.manoObra += o.manoObra;
        f.repuestos += calcularSubtotalRepuestos(o);
    }
    return E_OK;
}

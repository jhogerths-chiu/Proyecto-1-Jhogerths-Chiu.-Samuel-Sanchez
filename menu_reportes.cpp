// CAPA DE PRESENTACION - Reportes. Leen desde los archivos (a traves de la logica).
#include "taller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

static void reporteHistorialVehiculo() {
    cout << "\n--- REPORTE 1: HISTORIAL DE VEHICULO (escriba CANCELAR para abortar) ---\n";
    char placa[100];
    if (!pedirTexto("Placa del vehiculo: ", placa, sizeof placa, nullptr, "")) return;
    Vehiculo v;
    CodigoError e = obtenerVehiculoPorPlaca(placa, v);
    if (e == E_VEHICULO_NO_EXISTE) { cout << "ERROR: No existe un vehiculo activo con la placa '" << placa << "'.\n"; return; }
    if (e != E_OK) { mostrarErrorSistema(e); return; }

    cout << "\nDatos del vehiculo:\n";
    mostrarVehiculo(v);

    OrdenServicio res[MAX_RESULTADOS];
    int cantidad = 0;
    bool hayMas = false;
    e = listarOrdenesPorVehiculo(v.id, res, cantidad, hayMas);
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cantidad == 0) { cout << "\nEl vehiculo no tiene ordenes registradas.\n"; return; }

    int w[5] = {6, 10, 22, 8, 10};
    cout << "\n";
    lineaBorde(w, 5, '-');
    printf("| %-6s | %-10s | %-22s | %-8s | %-10s |\n", "Orden", "Fecha", "Mecanico", "Estado", "Total");
    lineaBorde(w, 5, '-');
    for (int i = 0; i < cantidad; i++) {
        Mecanico m;
        memset(&m, 0, sizeof m);
        obtenerMecanicoPorID(res[i].idMecanico, m);
        char nombre[110];
        snprintf(nombre, sizeof nombre, "%s%s", m.nombre[0] ? m.nombre : "?", m.eliminado ? " (BAJA)" : "");
        printf("| #%-5d | %-10s | %-22.22s | %-8s | %10.2f |\n", res[i].id, res[i].fechaApertura,
               nombre, res[i].estado, calcularTotalOrden(res[i]));
    }
    lineaBorde(w, 5, '-');
    if (hayMas) printf("AVISO: Hay mas ordenes. Se muestran solo las primeras %d.\n", MAX_RESULTADOS);

    float total = 0;
    e = totalFacturadoVehiculo(v.id, total);   // incluye TODAS las cerradas, aunque no se listen todas
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    printf("Total facturado en ordenes CERRADAS: %.2f\n", total);
}

static void reporteRepuestosMasUsados() {
    cout << "\n--- REPORTE 2: REPUESTOS MAS UTILIZADOS (ordenes CERRADAS) ---\n";
    UsoRepuesto top[TOP_REPUESTOS];
    int cantidad = 0;
    CodigoError e = repuestosMasUtilizados(top, cantidad);
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cantidad == 0) { cout << "No hay repuestos consumidos en ordenes cerradas.\n"; return; }

    int w[4] = {3, 10, 30, 9};
    lineaBorde(w, 4, '-');
    printf("| %-3s | %-10s | %-30s | %-9s |\n", "#", "Codigo", "Repuesto", "Unidades");
    lineaBorde(w, 4, '-');
    for (int i = 0; i < cantidad; i++) {
        Repuesto r;
        memset(&r, 0, sizeof r);
        obtenerRepuestoPorID(top[i].idRepuesto, r);   // incluso eliminado: rellena el registro
        printf("| %-3d | %-10.10s | %-30.30s | %-9d |\n", i + 1, r.codigo[0] ? r.codigo : "?",
               r.nombre[0] ? r.nombre : "?", top[i].unidades);
    }
    lineaBorde(w, 4, '-');
}

static void reporteFacturacionMecanico() {
    cout << "\n--- REPORTE 3: FACTURACION POR MECANICO (ordenes CERRADAS) ---\n";
    FacturacionMecanico filas[MAX_RESULTADOS];
    int cantidad = 0;
    bool hayMas = false;
    CodigoError e = facturacionPorMecanico(filas, cantidad, hayMas);
    if (e != E_OK) { mostrarErrorSistema(e); return; }
    if (cantidad == 0) { cout << "No hay mecanicos registrados.\n"; return; }

    int w[4] = {22, 8, 10, 10};
    lineaBorde(w, 4, '-');
    printf("| %-22s | %-8s | %-10s | %-10s |\n", "Mecanico", "Ordenes", "M. obra", "Repuestos");
    lineaBorde(w, 4, '-');
    float tMO = 0, tRep = 0;
    int tOrd = 0;
    for (int i = 0; i < cantidad; i++) {
        Mecanico m;
        memset(&m, 0, sizeof m);
        obtenerMecanicoPorID(filas[i].idMecanico, m);
        char nombre[130];
        snprintf(nombre, sizeof nombre, "%s%s", m.nombre, m.eliminado ? " (BAJA)" : "");
        printf("| %-22.22s | %-8d | %10.2f | %10.2f |\n", nombre, filas[i].ordenesCerradas,
               filas[i].manoObra, filas[i].repuestos);
        tOrd += filas[i].ordenesCerradas;
        tMO += filas[i].manoObra;
        tRep += filas[i].repuestos;
    }
    lineaBorde(w, 4, '-');
    printf("| %-22s | %-8d | %10.2f | %10.2f |\n", "TOTAL", tOrd, tMO, tRep);
    lineaBorde(w, 4, '-');
    if (hayMas) printf("AVISO: Hay mas mecanicos. Se muestran solo los primeros %d.\n", MAX_RESULTADOS);
}

void menuReportes() {
    const char* ops[3] = {"Historial de vehiculo", "Repuestos mas utilizados", "Facturacion por mecanico"};
    int op;
    do {
        imprimirMenu("REPORTES", nullptr, ops, 3, "Volver al menu principal");
        if (!pedirEntero("Seleccione una opcion: ", &op, 0, 3)) return;
        switch (op) {
            case 1: reporteHistorialVehiculo(); break;
            case 2: reporteRepuestosMasUsados(); break;
            case 3: reporteFacturacionMecanico(); break;
        }
    } while (op != 0);
}

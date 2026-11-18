// CAPA DE PRESENTACION - Mensajes comunes para los fallos de archivo.
#include "taller.h"
#include <iostream>
using namespace std;

void mostrarErrorSistema(CodigoError e) {
    switch (e) {
        case E_ARCHIVO:
            cout << "ERROR: No se pudo abrir o escribir el archivo '" << archivoConError()
                 << "'. La operacion no continua.\n";
            break;
        case E_ARCHIVO_CORRUPTO:
            cout << "ERROR: El archivo '" << archivoConError()
                 << "' esta incompleto o danado. La operacion no continua.\n";
            break;
        case E_DATOS_INVALIDOS:
            cout << "ERROR: Datos invalidos.\n";
            break;
        default:
            cout << "ERROR: La operacion no pudo completarse.\n";
            break;
    }
}

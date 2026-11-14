#ifndef TALLER_H
#define TALLER_H

// ============================================================
//  ESTRUCTURAS (segun enunciado: no se renombra ningun campo)
// ============================================================
struct Taller {
    char nombre[100];
    char rif[20];
    char direccion[200];
    char telefono[20];
};

struct Vehiculo {
    int id;
    char placa[10];
    char marca[50];
    char modelo[50];
    int anio;
    int kilometraje;
    char nombrePropietario[100];
    char cedulaPropietario[20];
    char telefonoPropietario[20];
    char fechaRegistro[11];
};

struct Mecanico {
    int id;
    char nombre[100];
    char cedula[20];
    char especialidad[20];
    char fechaIngreso[11];
};

struct Repuesto {
    int id;
    char codigo[20];
    char nombre[100];
    char marca[50];
    float precio;
    int stock;
    int stockMinimo;
    char fechaRegistro[11];
};

struct DetalleOrden {
    int idRepuesto;
    int cantidad;
    float precioUnitario;   // precio AL MOMENTO de agregar el repuesto
};

struct OrdenServicio {
    int id;
    int idVehiculo;
    int idMecanico;
    char fechaApertura[11];
    char fechaCierre[11];
    char estado[12];        // "ABIERTA", "CERRADA" o "ANULADA"
    char descripcionFalla[200];
    int kilometrajeIngreso;
    float manoObra;
    // Arreglo dinamico propio de cada orden
    DetalleOrden* detalles;
    int numDetalles;
    int capacidadDetalles;
};

struct SistemaTaller {
    Taller taller;
    Vehiculo* vehiculos;    int numVehiculos;  int capacidadVehiculos;
    Mecanico* mecanicos;    int numMecanicos;  int capacidadMecanicos;
    Repuesto* repuestos;    int numRepuestos;  int capacidadRepuestos;
    OrdenServicio* ordenes; int numOrdenes;    int capacidadOrdenes;
    int siguienteIdVehiculo;
    int siguienteIdMecanico;
    int siguienteIdRepuesto;
    int siguienteIdOrden;
};

// ============================================================
//  CODIGOS DE RESULTADO (la logica NO imprime: devuelve el motivo
//  del fallo y la presentacion decide que mensaje mostrar)
// ============================================================
enum CodigoError {
    E_OK = 0,
    E_DATOS_INVALIDOS,
    E_DUPLICADO,
    E_VEHICULO_NO_EXISTE,
    E_MECANICO_NO_EXISTE,
    E_REPUESTO_NO_EXISTE,
    E_ORDEN_NO_EXISTE,
    E_VEHICULO_CON_ORDEN_ABIERTA,
    E_MECANICO_LLENO,
    E_KM_MENOR,
    E_ORDEN_NO_ABIERTA,
    E_ORDEN_NO_ANULABLE,
    E_CANTIDAD_INVALIDA,
    E_STOCK_INSUFICIENTE,
    E_REPUESTO_NO_EN_ORDEN,
    E_ORDEN_SIN_CONTENIDO,
    E_MANO_OBRA_INVALIDA,
    E_PRECIO_INVALIDO,
    E_REPUESTO_EN_USO
};

// ============================================================
//  UTILIDAD GENERICA: duplica la capacidad de un arreglo dinamico
//  cuando esta lleno (num == capacidad). Se usa para vehiculos,
//  mecanicos, repuestos, ordenes y los detalles de cada orden.
// ============================================================
template <typename T>
void duplicarSiLleno(T*& arreglo, int numero, int& capacidad) {
    if (numero < capacidad) return;
    int nueva = capacidad * 2;
    T* nuevo = new T[nueva];
    for (int i = 0; i < numero; i++) nuevo[i] = arreglo[i];
    delete[] arreglo;
    arreglo = nuevo;
    capacidad = nueva;
}

// ============================================================
//  CAPA DE LOGICA (sin cin / cout)
// ============================================================

// --- sistema.cpp ---
void inicializarSistema(SistemaTaller* s, Taller datosTaller);
void liberarSistema(SistemaTaller* s);

// --- utilidades.cpp ---
void copiarTexto(char* dest, const char* src, int tam);
void aMayusculas(char* t);
bool igualesIgnorandoMayusculas(const char* a, const char* b);
bool contieneIgnorandoMayusculas(const char* texto, const char* sub);
bool textoNoVacio(const char* t);
bool cedulaValida(const char* c);        // solo digitos, prefijo V- E- J- opcional
bool placaValida(const char* p);         // alfanumerica, 6 o 7 caracteres
bool fechaValida(const char* f);         // YYYY-MM-DD real
bool especialidadValida(const char* e);  // MOTOR FRENOS ELECTRICIDAD SUSPENSION GENERAL
void fechaActual(char* dest);            // dest con al menos 11 bytes

// --- vehiculos.cpp ---
Vehiculo*  agregarVehiculo(SistemaTaller* s, Vehiculo datos);
Vehiculo*  buscarVehiculoPorID(SistemaTaller* s, int id);
Vehiculo*  buscarVehiculoPorPlaca(SistemaTaller* s, const char* placa);
Vehiculo** buscarVehiculosPorPropietario(SistemaTaller* s, const char* subcadena, int* cantidad);
Vehiculo** listarVehiculos(SistemaTaller* s, int* cantidad);
bool       actualizarVehiculo(SistemaTaller* s, int id, Vehiculo datos);
bool       eliminarVehiculo(SistemaTaller* s, int id);

// --- mecanicos.cpp ---
Mecanico*  agregarMecanico(SistemaTaller* s, Mecanico datos, CodigoError* error);
Mecanico*  buscarMecanicoPorID(SistemaTaller* s, int id);
Mecanico** listarMecanicos(SistemaTaller* s, int* cantidad);
int        contarOrdenesAbiertasMecanico(SistemaTaller* s, int idMecanico);

// --- repuestos.cpp ---
Repuesto*  agregarRepuesto(SistemaTaller* s, Repuesto datos, CodigoError* error);
Repuesto*  buscarRepuestoPorID(SistemaTaller* s, int id);
Repuesto*  buscarRepuestoPorCodigo(SistemaTaller* s, const char* codigo);
Repuesto** buscarRepuestosPorTexto(SistemaTaller* s, const char* subcadena, int* cantidad);
Repuesto** listarRepuestos(SistemaTaller* s, int* cantidad);
CodigoError registrarEntradaMercancia(SistemaTaller* s, int idRepuesto, int cantidad);
CodigoError cambiarPrecioRepuesto(SistemaTaller* s, int idRepuesto, float nuevoPrecio);
CodigoError eliminarRepuesto(SistemaTaller* s, int idRepuesto);
bool       estaBajoStock(const Repuesto* r);
bool       repuestoUsadoEnOrdenes(SistemaTaller* s, int idRepuesto);

// --- ordenes.cpp ---
OrdenServicio*  abrirOrden(SistemaTaller* s, int idVehiculo, int idMecanico,
                           int kilometrajeIngreso, const char* descripcionFalla,
                           CodigoError* error);
OrdenServicio*  buscarOrdenPorID(SistemaTaller* s, int id);
OrdenServicio*  ordenAbiertaDeVehiculo(SistemaTaller* s, int idVehiculo);
CodigoError     agregarRepuestoAOrden(SistemaTaller* s, int idOrden, int idRepuesto, int cantidad);
CodigoError     quitarRepuestoDeOrden(SistemaTaller* s, int idOrden, int idRepuesto);
CodigoError     cerrarOrden(SistemaTaller* s, int idOrden, float manoObra);
CodigoError     anularOrden(SistemaTaller* s, int idOrden);
float           calcularSubtotalRepuestos(const OrdenServicio* o);
float           calcularTotalOrden(const OrdenServicio* o);
int             contarOrdenesDeVehiculo(SistemaTaller* s, int idVehiculo);
OrdenServicio** listarOrdenes(SistemaTaller* s, int* cantidad);
OrdenServicio** listarOrdenesPorEstado(SistemaTaller* s, const char* estado, int* cantidad);
OrdenServicio** listarOrdenesPorVehiculo(SistemaTaller* s, int idVehiculo, int* cantidad);

// ============================================================
//  CAPA DE PRESENTACION (cin / cout)
// ============================================================

// --- entrada.cpp: lectura robusta (todas aceptan CANCELAR y detectan fin de entrada) ---
extern bool entradaTerminada;
bool pedirTexto(const char* prompt, char* dest, int tam,
                bool (*valida)(const char*), const char* msgInvalido);
bool pedirEntero(const char* prompt, int* out, int min, int max);
bool pedirDecimal(const char* prompt, float* out, float min, bool estricto);
bool pedirConfirmacion(const char* prompt);
void lineaBorde(const int* anchos, int n, char relleno);
void imprimirMenu(const char* titulo, const char* subtitulo,
                  const char* const* opciones, int n, const char* textoCero);

// --- menu_vehiculos.cpp ---
void menuGestionVehiculos(SistemaTaller* s);
void menuRegistrarVehiculo(SistemaTaller* s);
void menuBuscarVehiculo(SistemaTaller* s);
void menuActualizarVehiculo(SistemaTaller* s);
void menuEliminarVehiculo(SistemaTaller* s);
void menuListarVehiculos(SistemaTaller* s);
void mostrarVehiculo(Vehiculo* vehiculo);
void mostrarListaVehiculos(Vehiculo** vehiculos, int cantidad);

// --- menu_mecanicos.cpp ---
void menuGestionMecanicos(SistemaTaller* s);
void menuRegistrarMecanico(SistemaTaller* s);
void menuBuscarMecanico(SistemaTaller* s);
void menuListarMecanicos(SistemaTaller* s);
void mostrarMecanico(SistemaTaller* s, Mecanico* m);
void mostrarListaMecanicos(SistemaTaller* s, Mecanico** mecanicos, int cantidad);

// --- menu_repuestos.cpp ---
void menuGestionInventario(SistemaTaller* s);
void menuRegistrarRepuesto(SistemaTaller* s);
void menuBuscarRepuesto(SistemaTaller* s);
void menuEntradaMercancia(SistemaTaller* s);
void menuListarRepuestos(SistemaTaller* s);
void menuCambiarPrecio(SistemaTaller* s);
void menuEliminarRepuesto(SistemaTaller* s);
void mostrarRepuesto(Repuesto* r);
void mostrarListaRepuestos(Repuesto** repuestos, int cantidad);

// --- menu_ordenes.cpp ---
void menuGestionOrdenes(SistemaTaller* s);
void menuAbrirOrden(SistemaTaller* s);
void menuAgregarRepuestoOrden(SistemaTaller* s);
void menuQuitarRepuestoOrden(SistemaTaller* s);
void menuCerrarOrden(SistemaTaller* s);
void menuAnularOrden(SistemaTaller* s);
void menuVerDetalleOrden(SistemaTaller* s);
void menuListarOrdenes(SistemaTaller* s);
void mostrarDetalleOrden(SistemaTaller* s, OrdenServicio* orden);
void mostrarListaOrdenes(SistemaTaller* s, OrdenServicio** ordenes, int cantidad);

#endif

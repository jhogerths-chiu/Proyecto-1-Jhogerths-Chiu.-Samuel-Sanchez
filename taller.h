#ifndef TALLER_H
#define TALLER_H

#include <cstddef>
#include <ctime>

// ============================================================
//  CONSTANTES
// ============================================================
const int MAX_LINEAS_ORDEN = 20;   // lineas de detalle por orden (>= 15 exigido)
const int MAX_RESULTADOS   = 50;   // maximo de registros que entrega una busqueda/listado

const char* const CARPETA_DATOS     = "datos";
const char* const RUTA_TALLER       = "datos/taller.bin";
const char* const RUTA_VEHICULOS    = "datos/vehiculos.bin";
const char* const RUTA_MECANICOS    = "datos/mecanicos.bin";
const char* const RUTA_REPUESTOS    = "datos/repuestos.bin";
const char* const RUTA_ORDENES      = "datos/ordenes.bin";

// ============================================================
//  ESTRUCTURAS EN DISCO (tamano fijo: sin punteros ni std::string)
// ============================================================
struct ArchivoHeader {
    int cantidadRegistros;   // Total historico (incluye eliminados logicamente)
    int proximoID;           // Siguiente ID a asignar
    int registrosActivos;    // Registros con eliminado == false
    int version;             // Version del formato del archivo (inicia en 1)
};

struct Taller {
    char nombre[100];
    char rif[20];
    char direccion[200];
    char telefono[20];
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
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
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Mecanico {
    int id;
    char nombre[100];
    char cedula[20];
    char especialidad[20];
    char fechaIngreso[11];
    bool eliminado;          // true = dado de baja
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Repuesto {
    int id;
    char codigo[20];
    char nombre[100];
    char marca[50];
    float precio;
    int stock;
    int stockMinimo;
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// Una linea del detalle de una orden. Va DENTRO de la OrdenServicio (arreglo fijo).
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
    // DETALLE: arreglo de tamano fijo embebido en el registro
    DetalleOrden lineas[MAX_LINEAS_ORDEN];
    int numLineas;          // lineas realmente usadas (0..MAX_LINEAS_ORDEN)
    bool eliminado;         // Las ordenes no se eliminan: siempre false
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// ============================================================
//  CODIGOS DE RESULTADO (ni la persistencia ni la logica imprimen:
//  devuelven el motivo y la presentacion decide el mensaje)
// ============================================================
enum CodigoError {
    E_OK = 0,
    // --- archivos ---
    E_ARCHIVO,                       // no se pudo abrir / leer / escribir
    E_ARCHIVO_CORRUPTO,              // contenido inconsistente (truncado, header incoherente)
    E_ID_NO_EXISTE,                  // el ID no esta en el archivo
    E_ELIMINADO,                     // el registro existe pero esta eliminado/dado de baja
    // --- negocio ---
    E_DATOS_INVALIDOS,
    E_DUPLICADO,
    E_VEHICULO_NO_EXISTE,
    E_MECANICO_NO_EXISTE,
    E_REPUESTO_NO_EXISTE,
    E_ORDEN_NO_EXISTE,
    E_VEHICULO_CON_ORDENES,
    E_MECANICO_CON_ORDENES_ABIERTAS,
    E_CANTIDAD_INVALIDA,
    E_PRECIO_INVALIDO,
    E_REPUESTO_EN_USO,
    // --- ordenes ---
    E_VEHICULO_INACTIVO,             // vehiculo eliminado
    E_MECANICO_INACTIVO,             // mecanico dado de baja
    E_VEHICULO_CON_ORDEN_ABIERTA,
    E_MECANICO_LLENO,                // ya tiene 3 ordenes ABIERTAS
    E_KM_MENOR,
    E_ORDEN_NO_ABIERTA,
    E_ORDEN_NO_ANULABLE,
    E_STOCK_INSUFICIENTE,
    E_REPUESTO_NO_EN_ORDEN,
    E_ORDEN_SIN_CONTENIDO,
    E_MANO_OBRA_INVALIDA,
    E_LINEAS_LLENAS                  // la orden alcanzo MAX_LINEAS_ORDEN
};

// ============================================================
//  ESTRUCTURAS DE RESULTADO (en memoria; no se escriben en disco)
// ============================================================
const int MAX_PROBLEMAS  = 200;    // mensajes que guarda el reporte de integridad
const int TOP_REPUESTOS  = 10;

struct ReporteIntegridad {
    int vehiculosVerificados;
    int mecanicosVerificados;
    int repuestosVerificados;
    int ordenesVerificadas;
    int problemas;                         // total encontrado (puede superar MAX_PROBLEMAS)
    int numMensajes;                       // mensajes almacenados
    char mensajes[MAX_PROBLEMAS][160];
};

struct UsoRepuesto {
    int idRepuesto;
    int unidades;
};

struct FacturacionMecanico {
    int idMecanico;
    int ordenesCerradas;
    float manoObra;
    float repuestos;
};

// ============================================================
//  CAPA DE PERSISTENCIA (sin cin / cout, sin reglas de negocio)
// ============================================================

// --- archivos.cpp: header, taller y utilidades de disco ---
long        posicionRegistro(size_t tamRegistro, int indice);   // sizeof(ArchivoHeader) + indice * tam
CodigoError inicializarArchivo(const char* ruta);
CodigoError leerHeader(const char* ruta, ArchivoHeader& h);
CodigoError escribirHeader(const char* ruta, const ArchivoHeader& h);
bool        existeTaller();
CodigoError leerTaller(Taller& t);
CodigoError guardarTaller(Taller& t);
CodigoError errorArchivo(const char* ruta);       // registra el archivo con falla y retorna E_ARCHIVO
CodigoError errorCorrupto(const char* ruta);      // registra el archivo con falla y retorna E_ARCHIVO_CORRUPTO
const char* archivoConError();                    // ultimo archivo que fallo
bool        crearCarpeta(const char* ruta);
CodigoError contarRegistrosEnArchivo(const char* ruta, size_t tamRegistro, int& cantidad); // segun el tamano real
CodigoError copiarArchivo(const char* origen, const char* destino);                       // byte a byte
void        formatearFecha(time_t t, char* dest); // YYYY-MM-DD, dest con al menos 11 bytes

// --- persistencia.cpp: las 5 operaciones por entidad ---
// El ID se deduce del indice fisico: indice = id - 1 (ver README_V2).
CodigoError guardarVehiculo(Vehiculo& v);                                  // asigna v.id
CodigoError leerVehiculoPorID(int id, Vehiculo& out, long* posicion = nullptr);
CodigoError sobrescribirVehiculo(Vehiculo& v);
CodigoError marcarVehiculoEliminado(int id);
CodigoError contarVehiculosActivos(int& cantidad);

CodigoError guardarMecanico(Mecanico& m);
CodigoError leerMecanicoPorID(int id, Mecanico& out, long* posicion = nullptr);
CodigoError sobrescribirMecanico(Mecanico& m);
CodigoError marcarMecanicoEliminado(int id);
CodigoError contarMecanicosActivos(int& cantidad);

CodigoError guardarRepuesto(Repuesto& r);
CodigoError leerRepuestoPorID(int id, Repuesto& out, long* posicion = nullptr);
CodigoError sobrescribirRepuesto(Repuesto& r);
CodigoError marcarRepuestoEliminado(int id);
CodigoError contarRepuestosActivos(int& cantidad);

CodigoError guardarOrden(OrdenServicio& o);
CodigoError leerOrdenPorID(int id, OrdenServicio& out, long* posicion = nullptr);
CodigoError sobrescribirOrden(OrdenServicio& o);
CodigoError marcarOrdenEliminada(int id);
CodigoError contarOrdenesActivas(int& cantidad);

// ============================================================
//  CAPA DE LOGICA (sin cin / cout)
// ============================================================

// --- sistema.cpp ---
CodigoError inicializarSistema();        // crea datos/ y los archivos que falten

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
// Las busquedas multiples llenan 'res' (capacidad MAX_RESULTADOS); hayMas = true si habia mas.
CodigoError registrarVehiculo(Vehiculo datos, int* idNuevo);               // E_DATOS_INVALIDOS, E_DUPLICADO
CodigoError obtenerVehiculoPorID(int id, Vehiculo& out, long* posicion = nullptr); // E_ID_NO_EXISTE, E_ELIMINADO
CodigoError obtenerVehiculoPorPlaca(const char* placa, Vehiculo& out);     // E_VEHICULO_NO_EXISTE
CodigoError buscarVehiculosPorPropietario(const char* subcadena, Vehiculo* res, int& cantidad, bool& hayMas);
CodigoError listarVehiculos(Vehiculo* res, int& cantidad, bool& hayMas);
CodigoError actualizarVehiculo(int id, Vehiculo datos);
CodigoError eliminarVehiculo(int id);                                      // E_VEHICULO_CON_ORDENES

// --- mecanicos.cpp ---
CodigoError registrarMecanico(Mecanico datos, int* idNuevo);               // E_DATOS_INVALIDOS, E_DUPLICADO
CodigoError obtenerMecanicoPorID(int id, Mecanico& out, long* posicion = nullptr); // E_ELIMINADO = dado de baja (out lleno)
CodigoError listarMecanicos(Mecanico* res, int& cantidad, bool& hayMas);   // activos y de baja
CodigoError darDeBajaMecanico(int id);                                     // E_MECANICO_CON_ORDENES_ABIERTAS

// --- repuestos.cpp ---
CodigoError registrarRepuesto(Repuesto datos, int* idNuevo);               // E_DATOS_INVALIDOS, E_DUPLICADO
CodigoError obtenerRepuestoPorID(int id, Repuesto& out, long* posicion = nullptr); // E_ELIMINADO (out lleno)
CodigoError obtenerRepuestoPorCodigo(const char* codigo, Repuesto& out);   // E_REPUESTO_NO_EXISTE
CodigoError buscarRepuestosPorTexto(const char* subcadena, Repuesto* res, int& cantidad, bool& hayMas);
CodigoError listarRepuestos(Repuesto* res, int& cantidad, bool& hayMas);
CodigoError registrarEntradaMercancia(int idRepuesto, int cantidad);
CodigoError cambiarPrecioRepuesto(int idRepuesto, float nuevoPrecio);
CodigoError eliminarRepuesto(int idRepuesto);                              // E_REPUESTO_EN_USO
bool        estaBajoStock(const Repuesto& r);

// --- ordenes.cpp ---
CodigoError abrirOrden(int idVehiculo, int idMecanico, int kilometrajeIngreso,
                       const char* descripcionFalla, int* idNuevo);
CodigoError obtenerOrdenPorID(int id, OrdenServicio& out, long* posicion = nullptr); // E_ID_NO_EXISTE
CodigoError agregarRepuestoAOrden(int idOrden, int idRepuesto, int cantidad);
CodigoError quitarRepuestoDeOrden(int idOrden, int idRepuesto);
CodigoError cerrarOrden(int idOrden, float manoObra);
CodigoError anularOrden(int idOrden);
float       calcularSubtotalRepuestos(const OrdenServicio& o);
float       calcularTotalOrden(const OrdenServicio& o);
CodigoError ordenAbiertaDeVehiculo(int idVehiculo, int& idOrden);          // idOrden = 0 si no hay
CodigoError listarOrdenes(OrdenServicio* res, int& cantidad, bool& hayMas);
CodigoError listarOrdenesPorEstado(const char* estado, OrdenServicio* res, int& cantidad, bool& hayMas);
CodigoError listarOrdenesPorVehiculo(int idVehiculo, OrdenServicio* res, int& cantidad, bool& hayMas);
CodigoError contarOrdenesDeVehiculo(int idVehiculo, int& cantidad);        // cualquier estado
CodigoError contarOrdenesAbiertasMecanico(int idMecanico, int& cantidad);
CodigoError repuestoUsadoEnOrdenes(int idRepuesto, bool& usado);           // aparece en alguna linea

// --- mantenimiento.cpp ---
CodigoError verificarIntegridad(ReporteIntegridad& reporte);
CodigoError crearBackup(char* rutaBackup, int tam);                        // retorna la ruta creada

// --- reportes.cpp ---
CodigoError totalFacturadoVehiculo(int idVehiculo, float& total);         // solo ordenes CERRADAS
CodigoError repuestosMasUtilizados(UsoRepuesto* top, int& cantidad);       // hasta TOP_REPUESTOS
CodigoError facturacionPorMecanico(FacturacionMecanico* filas, int& cantidad, bool& hayMas);

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

// --- mensajes.cpp ---
void mostrarErrorSistema(CodigoError e);   // fallos de archivo y datos invalidos genericos

// --- menu_vehiculos.cpp ---
void menuGestionVehiculos();
void menuRegistrarVehiculo();
void menuBuscarVehiculo();
void menuActualizarVehiculo();
void menuEliminarVehiculo();
void menuListarVehiculos();
void mostrarVehiculo(const Vehiculo& v);
void mostrarListaVehiculos(const Vehiculo* vehiculos, int cantidad, bool hayMas);

// --- menu_mecanicos.cpp ---
void menuGestionMecanicos();
void menuRegistrarMecanico();
void menuBuscarMecanico();
void menuListarMecanicos();
void menuDarDeBajaMecanico();
void mostrarMecanico(const Mecanico& m);
void mostrarListaMecanicos(const Mecanico* mecanicos, int cantidad, bool hayMas);

// --- menu_repuestos.cpp ---
void menuGestionInventario();
void menuRegistrarRepuesto();
void menuBuscarRepuesto();
void menuEntradaMercancia();
void menuListarRepuestos();
void menuCambiarPrecio();
void menuEliminarRepuesto();
void mostrarRepuesto(const Repuesto& r);
void mostrarListaRepuestos(const Repuesto* repuestos, int cantidad, bool hayMas);

// --- menu_ordenes.cpp ---
void menuGestionOrdenes();
void menuAbrirOrden();
void menuAgregarRepuestoOrden();
void menuQuitarRepuestoOrden();
void menuCerrarOrden();
void menuAnularOrden();
void menuVerDetalleOrden();
void menuListarOrdenes();
void mostrarDetalleOrden(const OrdenServicio& o);
void mostrarListaOrdenes(const OrdenServicio* ordenes, int cantidad, bool hayMas);

// --- menu_reportes.cpp / menu_mantenimiento.cpp ---
void menuReportes();
void menuMantenimiento();

#endif

# README_V2 — Proyecto 2: Taller mecánico con persistencia y acceso aleatorio

Programación 2 · 2026-C · Universidad Rafael Urdaneta

## 1. Compilación y ejecución

Todos los `.cpp` están en la raíz del proyecto.

```
g++ -std=c++11 -Wall -Wextra -o taller *.cpp
./taller          (Windows: taller.exe)
```

- Al arrancar, el programa crea la carpeta `datos/` y los archivos `.bin` que falten (con su header inicial).
- Si `datos/taller.bin` no existe (primer arranque) pide los datos del taller; en los siguientes solo los lee.
- La carpeta `datos/` entregada ya viene **pre-poblada**: 8 vehículos, 4 mecánicos (uno dado de baja), 12 repuestos y 10 órdenes (6 cerradas, 2 abiertas, 2 anuladas; 3 con 4 o más líneas). Para regenerarla: borrar `datos/` y ejecutar `./taller < pruebas/entrada_0_poblar.txt`.
- Pruebas: `pruebas\ejecutar_pruebas.bat` (Windows) ejecuta los 8 casos de uso en orden, escribe `pruebas/salida_*.txt` y deja `datos/` pre-poblado. Las entradas por ejecución están numeradas (`entrada_3a.txt`, `entrada_3b.txt`, …).

Estructura por capas (igual que el Proyecto 1, sin `SistemaTaller` ni arreglos en RAM):

| Capa | Archivos | Reglas |
|---|---|---|
| Persistencia | `archivos.cpp`, `persistencia.cpp` | Solo `<fstream>`, sin `cin/cout`, sin reglas de negocio, no filtra eliminados |
| Lógica | `vehiculos.cpp`, `mecanicos.cpp`, `repuestos.cpp`, `ordenes.cpp`, `reportes.cpp`, `mantenimiento.cpp`, `sistema.cpp`, `utilidades.cpp` | Reglas de negocio; retorna `CodigoError`; sin `cin/cout` |
| Presentación | `main.cpp`, `entrada.cpp`, `mensajes.cpp`, `menu_*.cpp` | `cin/cout`; nunca abre archivos |

## 2. `sizeof` de las estructuras y fórmula de posición

Medidos con g++ de 64 bits (`time_t` = 8 bytes). Imprimirlos con `sizeof` en otra plataforma puede dar valores distintos por el tamaño de `time_t` y el relleno (padding).

| Estructura | sizeof (bytes) |
|---|---|
| `ArchivoHeader` | 16 |
| `Taller` | 360 |
| `Vehiculo` | 288 |
| `Mecanico` | 176 |
| `Repuesto` | 208 |
| `DetalleOrden` | 12 |
| `OrdenServicio` | 520 |

Fórmula usada en **todas** las lecturas y escrituras por ID:

```
posicion = sizeof(ArchivoHeader) + (id - 1) * sizeof(Estructura)
```

Ejemplo: vehículo ID 5 → 16 + 4 × 288 = 1168 bytes (el programa lo muestra como mensaje `[DEPURACION]` al buscar por ID).
Tamaño de un archivo = 16 + cantidadRegistros × sizeof(Estructura) (p. ej. `ordenes.bin` con 10 órdenes = 5216 bytes).

## 3. Relación entre ID e índice físico

**`indice = id - 1`.** Los IDs se asignan desde 1 en orden de inserción, los registros se agregan siempre al final, se modifican en su lugar y el borrado es solo lógico; por eso el registro con ID *n* está siempre en la posición *n − 1* y no hace falta recorrer el archivo.

La relación **dejaría de ser válida** si cambia cualquiera de estos supuestos:
- borrado **físico** con compactación (los registros siguientes se correrían una posición);
- reordenar el archivo (por ejemplo, ordenarlo por placa);
- asignar IDs que no sean consecutivos (saltos, reutilización de IDs, IDs que no empiezan en 1 o que el usuario elige);
- insertar registros en medio del archivo en lugar de al final.

En ese caso habría que guardar un índice (ID → posición) o buscar secuencialmente. Como salvaguarda, `guardarX` verifica `proximoID == cantidadRegistros + 1` y, si no se cumple, retorna `E_ARCHIVO_CORRUPTO`.

## 4. Diseño del detalle de la orden

### Estructura elegida: arreglo fijo embebido en el registro

```cpp
struct DetalleOrden { int idRepuesto; int cantidad; float precioUnitario; };   // 12 bytes

struct OrdenServicio {
    ... campos de la orden ...
    DetalleOrden lineas[MAX_LINEAS_ORDEN];   // MAX_LINEAS_ORDEN = 20
    int numLineas;                           // lineas usadas
    bool eliminado; time_t fechaCreacion; time_t fechaUltimaModificacion;
};
```

No se necesita un archivo adicional: las líneas viajan dentro de `ordenes.bin`, así que el backup y la verificación de integridad ya las cubren.

### Cumplimiento de los requisitos de la sección 3
1. **Persistencia de las líneas:** cada línea guarda `idRepuesto`, `cantidad` y `precioUnitario` (precio histórico) dentro de la orden; sobreviven entre ejecuciones (casos 3 y 4).
2. **Al menos 15 líneas:** admite 20 (`MAX_LINEAS_ORDEN`).
3. **Registros del mismo tamaño:** toda orden mide 520 bytes, con cualquier cantidad de líneas.
4. **Consultar líneas sin cargar otras órdenes:** `leerOrdenPorID` hace un `seekg` y lee 520 bytes; las líneas ya vienen incluidas.
5. **Reglas del Proyecto 1:** se acumula en la línea existente, se quita la línea completa devolviendo su stock y el precio queda como el vigente al agregar la primera vez.

### Espacio en disco y desperdicio

| Orden con… | Bytes en disco | Bytes de líneas usados | Bytes desperdiciados en el arreglo |
|---|---|---|---|
| 1 línea | 520 | 12 | 19 × 12 = 228 |
| 15 líneas | 520 | 180 | 5 × 12 = 60 |
| 20 líneas (máximo) | 520 | 240 | 0 |

El arreglo de líneas ocupa 240 bytes por orden aunque no se use. Es el costo de tener tamaño fijo.

### Alternativas consideradas y por qué se descartaron
- **Archivo `detalles.bin` con una línea por registro** (`idOrden`, `idRepuesto`, `cantidad`, `precio`, más un enlace `siguiente`): no desperdicia espacio y no tiene tope de líneas, pero obtener las líneas de una orden exige recorrer una lista enlazada con varios `seekg` (o recorrer todo el archivo), quitar una línea obliga a reenlazar o a dejar huecos, y agrega un archivo más que mantener, respaldar y verificar. Además, con borrado lógico el archivo crecería sin control. Se descartó por complejidad y por más puntos de falla entre dos escrituras.
- **Archivo `detalles.bin` con un bloque de tamaño fijo por orden** (mismo contenido que el arreglo embebido, pero separado): ofrece lo mismo que la solución elegida pero con dos archivos y una escritura más por operación; no aporta ventaja.
- **Registro de tamaño variable** (orden + sus líneas contiguas): incumple el requisito de que todos los registros midan lo mismo y rompe el cálculo de posición.

### Cómo se resuelve «quitar una línea»
`quitarRepuestoDeOrden` busca la línea por `idRepuesto`, **desplaza hacia la izquierda** las líneas siguientes (así las líneas usadas siempre quedan contiguas desde el índice 0), disminuye `numLineas`, limpia la última posición y sobrescribe el registro de la orden. Después devuelve las unidades al stock del repuesto.

## 5. Firmas de la capa de persistencia y de la lógica

**Criterio general:** toda función que puede fallar retorna `CodigoError` (`E_OK` si salió bien; si no, el motivo). Los datos se entregan por **referencia de salida** (copia del registro leído), nunca como puntero al sistema. Las búsquedas múltiples llenan un arreglo de capacidad `MAX_RESULTADOS` (50) y avisan con `bool& hayMas`. Los fallos de archivo guardan el nombre del archivo (`archivoConError()`) para que la presentación lo muestre.

### Persistencia (idéntica para Vehiculo, Mecanico, Repuesto y OrdenServicio)

| Función | Cómo informa éxito / fallo / causa |
|---|---|
| `CodigoError guardarX(X& r)` | Asigna `r.id` (el que llama lo conoce por la referencia), marcas de tiempo y `eliminado=false`. `E_OK`, `E_ARCHIVO`, `E_ARCHIVO_CORRUPTO` |
| `CodigoError leerXPorID(int id, X& out, long* posicion = nullptr)` | Cálculo directo. Entrega también eliminados. `E_ID_NO_EXISTE` si el ID está fuera de rango; `posicion` opcional para el mensaje de depuración |
| `CodigoError sobrescribirX(X& r)` | Escribe en su posición y actualiza `fechaUltimaModificacion`. `E_ID_NO_EXISTE`, `E_ARCHIVO` |
| `CodigoError marcarXEliminado(int id)` | Borrado lógico + `registrosActivos--`. `E_ELIMINADO` si ya lo estaba (no se descuenta dos veces) |
| `CodigoError contarXActivos(int& n)` | Lee solo el header |

Header (`archivos.cpp`): `inicializarArchivo(ruta)` (retorna `E_OK` si quedó listo y no modifica uno existente), `leerHeader(ruta, h)`, `escribirHeader(ruta, h)` (solo la posición 0). También: `existeTaller/leerTaller/guardarTaller`, `contarRegistrosEnArchivo` (según el tamaño real en bytes) y `copiarArchivo` (byte a byte).

### Lógica

| Módulo | Funciones y causas de fallo |
|---|---|
| Vehículos | `registrarVehiculo(datos, int* idNuevo)` → `E_DATOS_INVALIDOS`, `E_DUPLICADO`. `obtenerVehiculoPorID(id, out, pos)` → `E_ID_NO_EXISTE` o `E_ELIMINADO` (con `out` lleno, para informarlo «eliminado» y no «no existe»). `obtenerVehiculoPorPlaca` → `E_VEHICULO_NO_EXISTE`. `buscarVehiculosPorPropietario`, `listarVehiculos` (arreglo + `hayMas`). `actualizarVehiculo(id, datos)`. `eliminarVehiculo(id)` → `E_VEHICULO_CON_ORDENES` |
| Mecánicos | `registrarMecanico` → `E_DATOS_INVALIDOS`, `E_DUPLICADO` (cédula). `obtenerMecanicoPorID` → `E_ELIMINADO` = dado de baja (con `out` lleno). `listarMecanicos` (activos y de baja). `darDeBajaMecanico(id)` → `E_MECANICO_CON_ORDENES_ABIERTAS` |
| Repuestos | `registrarRepuesto`, `obtenerRepuestoPorID` (`E_ELIMINADO`), `obtenerRepuestoPorCodigo`, `buscarRepuestosPorTexto`, `listarRepuestos`, `registrarEntradaMercancia` (`E_CANTIDAD_INVALIDA`), `cambiarPrecioRepuesto` (`E_PRECIO_INVALIDO`), `eliminarRepuesto` (`E_REPUESTO_EN_USO`) |
| Órdenes | `abrirOrden` (`E_VEHICULO_NO_EXISTE`, `E_VEHICULO_INACTIVO`, `E_MECANICO_NO_EXISTE`, `E_MECANICO_INACTIVO`, `E_VEHICULO_CON_ORDEN_ABIERTA`, `E_MECANICO_LLENO`, `E_KM_MENOR`), `agregarRepuestoAOrden` (`E_ORDEN_NO_ABIERTA`, `E_STOCK_INSUFICIENTE`, `E_LINEAS_LLENAS`, …), `quitarRepuestoDeOrden` (`E_REPUESTO_NO_EN_ORDEN`), `cerrarOrden` (`E_MANO_OBRA_INVALIDA`, `E_ORDEN_SIN_CONTENIDO`), `anularOrden` (`E_ORDEN_NO_ANULABLE`), `calcularTotalOrden`, `listarOrdenes/PorEstado/PorVehiculo` |
| Mantenimiento | `verificarIntegridad(ReporteIntegridad&)`, `crearBackup(char* ruta, int tam)` |
| Reportes | `totalFacturadoVehiculo`, `repuestosMasUtilizados(UsoRepuesto*, int&)`, `facturacionPorMecanico(FacturacionMecanico*, int&, bool&)` |

La presentación traduce cada `CodigoError` a un mensaje con datos concretos (nombres, placas, estados), nunca con IDs sueltos.

## 6. Orden de escrituras en operaciones compuestas

Todas las validaciones se hacen **antes** de la primera escritura. Principio elegido: **el stock nunca debe quedar mayor que el real**. Por eso lo que *descuenta* stock se escribe primero y lo que *devuelve* stock se escribe al final.

| Operación | Orden de escritura |
|---|---|
| Abrir orden | 1) orden nueva 2) kilometraje del vehículo |
| Agregar repuesto | 1) stock del repuesto 2) detalle (la orden) |
| Quitar repuesto | 1) detalle (la orden) 2) stock del repuesto |
| Cerrar orden | solo la orden |
| Anular orden | 1) orden marcada ANULADA 2) stock de cada línea |

Si el programa se interrumpe entre ambas escrituras, queda stock **de menos** (unidades que ya no están en el inventario ni en una línea), nunca stock inventado. Es un daño seguro y se corrige con una entrada de mercancía.

## 7. Preguntas de análisis

> Respuestas escritas como borrador técnico. **Revísenlas y reescríbanlas con sus propias palabras** (el enunciado lo exige), y ejecuten ustedes mismos el experimento de la pregunta 1.

**1. Escribir una `OrdenServicio` del Proyecto 1 con su `DetalleOrden*` y volver a leerla.**
Con `write` solo se copian los `sizeof` bytes de la estructura. El puntero `detalles` es un número (una dirección de memoria) y eso es lo único que se guarda; las líneas en sí viven en el heap y no se escriben. Al volver a ejecutar el programa, esa dirección ya no corresponde a nada: el sistema operativo asignó la memoria de otra forma. Acceder a `detalles[i]` lee basura o provoca un fallo de segmentación. Por eso en disco las líneas deben estar en bytes de tamaño conocido dentro del archivo (aquí, un arreglo fijo dentro del registro).

**2. Interrupción entre la escritura del detalle y la del stock.**
- Si se escribe primero el **detalle** y se interrumpe antes del stock: la orden muestra una línea pero el stock sigue intacto. Hay unidades «inventadas»: el inventario cree tener más de lo que realmente hay y se podrían vender repuestos que no existen. Es difícil de detectar porque ningún dato es inválido por sí solo.
- Si se escribe primero el **stock**: el stock ya se descontó pero la orden no tiene la línea. Faltan unidades en el inventario sin que una orden las use. Es un daño conservador: nunca se vende lo que no hay, y se corrige registrando una entrada de mercancía.
Elegimos descontar primero porque el peor caso es perder unidades, no inventarlas. En «quitar» y «anular» el orden es el inverso (la orden primero, el stock al final) por la misma razón: devolver stock es lo último que se escribe.

**3. ¿Qué habría pasado con el historial si se hubiera permitido la eliminación física de un mecánico?**
Las órdenes CERRADAS y ANULADAS conservarían un `idMecanico` que apunta a un registro que ya no existe: no se podría mostrar quién hizo el trabajo, el reporte de facturación por mecánico perdería sus datos y la verificación de integridad marcaría las órdenes como rotas. Además, con borrado físico los registros siguientes se correrían y la fórmula `id − 1` dejaría de apuntar al registro correcto. La baja lógica conserva el registro con `eliminado = true`: se le impide recibir órdenes nuevas, pero su nombre sigue disponible para todo el historial.

## 8. Mantenimiento y reportes

- **Verificación de integridad** (menú 6 → 1): compara cada header con el contenido real del archivo (por tamaño en bytes y por conteo de activos), y revisa órdenes con vehículo/mecánico inexistente, órdenes ABIERTAS con vehículo o mecánico inactivo (en CERRADAS/ANULADAS un mecánico de baja es válido), líneas con repuesto inexistente y stock negativo. Cada problema se reporta con datos para ubicarlo y se cierra con el resumen del enunciado.
- **Respaldo** (menú 6 → 2): crea `backup_AAAA-MM-DD_HH-MM/` y copia byte a byte `taller.bin` y los cuatro `.bin` de entidades.
- **Reportes** (menú 5): historial de vehículo, 10 repuestos más utilizados y facturación por mecánico. Solo suman órdenes CERRADAS. Leen un registro de orden a la vez.

## 9. Casos de uso

| Caso | Archivos en `pruebas/` | Qué demuestra |
|---|---|---|
| 1 Persistencia | `entrada_1a`, `entrada_1b` | Datos y IDs iguales tras reiniciar |
| 2 Acceso directo | `entrada_2` | Lectura del ID 5 en la posición 1168 y aviso de ID 3 eliminado |
| 3 Orden con detalle | `entrada_3a`, `3b`, `3c` | Acumular, quitar con devolución de stock, cerrar y mostrar tras reiniciar |
| 4 Quince líneas | `entrada_4a`, `4b` | 15 repuestos distintos persisten |
| 5 Anulación | `entrada_5a`, `5b` | El stock vuelve a su valor previo en `repuestos.bin` |
| 6 Baja de mecánico | `entrada_6` | Rechazada con orden abierta, aceptada al cerrarla, historial con nombre, orden nueva rechazada |
| 7 Integridad | `entrada_7a`, `7b` | Detecta «Orden #13 apunta al vehículo ID 9» tras restaurar solo `vehiculos.bin` |
| 8 Reportes | `entrada_8` | Totales sin órdenes abiertas ni anuladas |

El caso 7 requiere reemplazar `datos/vehiculos.bin` por la copia del backup entre `7a` y `7b`; el `.bat` lo hace automáticamente.

## 10. Limitaciones conocidas

- Las listas y búsquedas entregan como máximo `MAX_RESULTADOS` (50) registros y avisan cuando hay más.
- Una orden admite hasta `MAX_LINEAS_ORDEN` (20) líneas distintas.
- Los archivos se guardan con el formato binario de la plataforma (tamaño de `time_t` y padding): no son portables entre compiladores distintos.
- Cada operación abre y cierra el archivo (simple y seguro; no optimizado para volúmenes grandes).

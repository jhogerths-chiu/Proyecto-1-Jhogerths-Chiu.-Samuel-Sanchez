# Sistema de Gestión de Taller Mecánico

Proyecto 1 de **Programación 2** (trimestre 2026-C) · Universidad Rafael Urdaneta · Prof. Ing. Victor Kneider

## Integrantes
* Jhogerths Shiloung Chiu Herrera
* Samuel Elías Sánchez Herández

## Descripción del sistema
Sistema en C++ para administrar un taller mecánico: **vehículos** (con los datos del propietario), **mecánicos**,
**inventario de repuestos** y **órdenes de servicio**. Usa memoria dinámica manual (`new` / `delete`), arreglos que
se redimensionan solos (la capacidad se duplica al llenarse), memoria dinámica anidada (cada orden tiene su propio
arreglo de detalles) y una separación estricta entre la capa de lógica y la de presentación.
No se usa `vector` ni ningún contenedor de la STL; `std::string` solo se usa para leer temporalmente la entrada.

## Compilación y ejecución
```bash
g++ -std=c++11 -Wall -Wextra -o taller main.cpp sistema.cpp utilidades.cpp vehiculos.cpp mecanicos.cpp repuestos.cpp ordenes.cpp entrada.cpp menu_vehiculos.cpp menu_mecanicos.cpp menu_repuestos.cpp menu_ordenes.cpp
./taller                                          # uso interactivo
./taller < pruebas/entrada.txt > pruebas/salida.txt   # ejecuta los 8 casos de uso
```
En Windows (PowerShell) ejecutar `.\taller.exe` y, para las pruebas,
`Get-Content pruebas\entrada.txt | .\taller.exe > pruebas\salida.txt` (o en `cmd`: `taller.exe < pruebas\entrada.txt > pruebas\salida.txt`).

Escribir `CANCELAR` en cualquier dato aborta la operación en curso. El programa no usa `getch` ni `system("pause")`.

## Organización de archivos
| Archivo | Capa | Contenido |
|---|---|---|
| `taller.h` | — | Estructuras, enum `CodigoError`, plantilla `duplicarSiLleno`, todos los prototipos |
| `sistema.cpp` | Lógica | `inicializarSistema`, `liberarSistema` |
| `utilidades.cpp` | Lógica | Validaciones (cédula, placa, fecha, especialidad) y manejo de texto |
| `vehiculos.cpp`, `mecanicos.cpp`, `repuestos.cpp`, `ordenes.cpp` | Lógica | CRUD y reglas de negocio de cada módulo |
| `entrada.cpp` | Presentación | Lectura robusta (`pedirTexto`, `pedirEntero`, ...), dibujo de menús y bordes de tablas |
| `menu_*.cpp` | Presentación | Menús, formularios y tablas de cada módulo |
| `main.cpp` | Presentación | Menú principal |

## Diagrama de estructuras
```
SistemaTaller
 ├─ Taller taller                      (único, sin CRUD)
 ├─ Vehiculo*       vehiculos  [num, capacidad]   ← arreglo dinámico
 ├─ Mecanico*       mecanicos  [num, capacidad]   ← arreglo dinámico
 ├─ Repuesto*       repuestos  [num, capacidad]   ← arreglo dinámico
 ├─ OrdenServicio*  ordenes    [num, capacidad]   ← arreglo dinámico
 │     └─ cada OrdenServicio:
 │          idVehiculo ───────────► Vehiculo.id
 │          idMecanico ───────────► Mecanico.id
 │          DetalleOrden* detalles [numDetalles, capacidadDetalles]  ← arreglo dinámico PROPIO
 │               └─ cada DetalleOrden: idRepuesto ──► Repuesto.id ; cantidad ; precioUnitario
 └─ siguienteIdVehiculo / Mecanico / Repuesto / Orden   (IDs autoincrementales desde 1)
```
Las relaciones se hacen por **ID** (no por puntero), de modo que redimensionar un arreglo nunca deja referencias rotas.

## Separación de capas
* **Lógica** (`sistema`, `utilidades`, `vehiculos`, `mecanicos`, `repuestos`, `ordenes`): opera sobre `SistemaTaller*`,
  **no contiene `cin` ni `cout`**. Retorna punteros a la entidad afectada, arreglos de punteros, `bool` o un `CodigoError`.
* **Presentación** (`entrada`, `menu_*`, `main`): lee datos, llama a la lógica y muestra el resultado. **No modifica
  datos directamente**: todo cambio pasa por una función de lógica. Los mensajes de error con datos concretos
  (`ERROR: Stock insuficiente de 'X'. Disponible: 2, solicitado: 5.`) se arman aquí a partir del `CodigoError` recibido.

## Reglas de diseño destacadas
* El total de una orden **no se almacena**: `calcularTotalOrden` = `manoObra` + Σ(`cantidad` × `precioUnitario`).
* Al agregar un repuesto que ya está en la orden se acumula en la línea existente (conserva el precio con el que se agregó).
* Al anular una orden se devuelve el stock pero se **conservan sus líneas** (historial y total consultables). Por eso un repuesto
  que aparece en alguna orden, aunque esté anulada, no puede eliminarse.
* "BAJO STOCK" = `stock <= stockMinimo`.
* Placas y cédulas se guardan en mayúsculas y se comparan sin distinguir mayúsculas/minúsculas.

## Firmas diseñadas (mecánicos, repuestos, órdenes)
Para los módulos libres se usa el enum `CodigoError`, que permite a la presentación distinguir **por qué** falló una operación
(el problema que plantea la pregunta frecuente sobre `eliminarVehiculo` con `bool`).

### Mecánicos
```cpp
Mecanico*  agregarMecanico(SistemaTaller* s, Mecanico datos, CodigoError* error);
Mecanico*  buscarMecanicoPorID(SistemaTaller* s, int id);
Mecanico** listarMecanicos(SistemaTaller* s, int* cantidad);        // el llamador hace delete[]
int        contarOrdenesAbiertasMecanico(SistemaTaller* s, int idMecanico);
```
* `agregarMecanico` retorna el puntero (igual que `agregarVehiculo`) y entrega el motivo del fallo (`E_DUPLICADO`, `E_DATOS_INVALIDOS`) por el parámetro `error`.
* `listarMecanicos` retorna `Mecanico**` siguiendo el patrón de `listarVehiculos`; las órdenes abiertas se consultan con `contarOrdenesAbiertasMecanico`, que también sirve para la regla de máximo 3.

### Repuestos
```cpp
Repuesto*   agregarRepuesto(SistemaTaller* s, Repuesto datos, CodigoError* error);
Repuesto*   buscarRepuestoPorID(SistemaTaller* s, int id);
Repuesto*   buscarRepuestoPorCodigo(SistemaTaller* s, const char* codigo);
Repuesto**  buscarRepuestosPorTexto(SistemaTaller* s, const char* subcadena, int* cantidad);
Repuesto**  listarRepuestos(SistemaTaller* s, int* cantidad);
CodigoError registrarEntradaMercancia(SistemaTaller* s, int idRepuesto, int cantidad);
CodigoError cambiarPrecioRepuesto(SistemaTaller* s, int idRepuesto, float nuevoPrecio);
CodigoError eliminarRepuesto(SistemaTaller* s, int idRepuesto);
bool        estaBajoStock(const Repuesto* r);
bool        repuestoUsadoEnOrdenes(SistemaTaller* s, int idRepuesto);
```
* Las operaciones que no producen una entidad nueva (entrada, cambio de precio, eliminar) retornan `CodigoError`: no hay un puntero natural que devolver y la presentación necesita saber si fue inexistente, cantidad inválida, precio inválido o repuesto en uso.
* `estaBajoStock` es una función aparte para que la regla viva en un solo lugar.

### Órdenes
```cpp
OrdenServicio*  abrirOrden(SistemaTaller* s, int idVehiculo, int idMecanico, int kilometrajeIngreso,
                           const char* descripcionFalla, CodigoError* error);
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
```
* `abrirOrden` crea una entidad nueva, así que retorna `OrdenServicio*` (o `nullptr`) y deja el motivo en `error` (vehículo con orden abierta, mecánico lleno, kilometraje menor, etc.).
* Agregar, quitar, cerrar y anular modifican una orden existente: retornan `CodigoError`, porque el mensaje depende del motivo (stock insuficiente, orden no abierta, repuesto no está en la orden...).
* `ordenAbiertaDeVehiculo` permite que el mensaje diga cuál orden (`#4`) bloquea al vehículo.
* `calcular*` retornan `float` y reciben `const`: no modifican nada.

## Preguntas de análisis
*(Respuestas redactadas con base en nuestra implementación; cada integrante debe poder explicarlas con sus propias palabras.)*

**1. `agregarVehiculo` retorna un puntero a un elemento de `s->vehiculos`. Si guardamos ese puntero, registramos más vehículos y luego lo usamos, ¿qué puede ocurrir y por qué?**
Puede quedar un *puntero colgante*. Cuando el arreglo se llena, `duplicarSiLleno` reserva un arreglo nuevo, copia los elementos y hace `delete[]` del arreglo viejo. El puntero guardado sigue apuntando a la memoria que ya se liberó, así que leer o escribir a través de él es comportamiento indefinido (datos basura, corrupción o caída del programa). Por eso en el sistema los punteros retornados se usan de inmediato y las relaciones entre entidades se guardan por **ID**, no por puntero.

**2. ¿Por qué `DetalleOrden` guarda `precioUnitario` en lugar de consultar el precio del repuesto cada vez que se calcula el total?**
Porque el precio de un repuesto puede cambiar con el tiempo (`cambiarPrecioRepuesto`). Si el total se calculara con el precio actual, una orden ya cerrada cambiaría de monto cada vez que se modifique el precio, y se falsearía el historial y lo que ya se le cobró al cliente. Guardando el precio al momento de agregar el repuesto, el total de cada orden queda fijo (Caso de Uso 4).

**3. Cuando se redimensiona el arreglo de órdenes, ¿qué ocurre con el arreglo de detalles de cada orden? ¿Se copia, se comparte o se pierde?**
**No se pierde y no se duplica: se transfiere el puntero.** `duplicarSiLleno` copia cada `OrdenServicio` con asignación de structs, lo que copia el valor del campo `detalles` (la dirección). El `delete[]` del arreglo viejo libera solo las estructuras `OrdenServicio`, no el bloque al que apuntaba `detalles`, porque `delete[]` no sigue punteros internos. Así, la orden nueva y la vieja apuntaban al mismo bloque, pero la vieja desaparece y queda un único dueño. Si en cambio hiciéramos `delete[] ordenes[i].detalles` antes de redimensionar, se perderían las líneas. Al final, `liberarSistema` recorre las órdenes y libera cada arreglo de detalles antes de liberar el arreglo de órdenes.

## Casos de uso
La carpeta `pruebas/` contiene `entrada.txt` (ejecuta los 8 casos de uso en orden: vehículos, inventario, orden completa, precio histórico, anulación, validaciones, redimensionamiento y cancelación) y `salida.txt` (generada con el comando indicado arriba).

# Sistema de Gestión de Taller Mecánico

Programación 2 · 2026-C · Universidad Rafael Urdaneta · Profesor: Ing. Victor Kneider

Aplicación de consola en C++ para gestionar un taller mecánico: vehículos, mecánicos, inventario de repuestos y órdenes de servicio.

## Estado del proyecto

| Proyecto | Descripción | Estado |
|---|---|---|
| Proyecto 1 | Sistema en memoria (arreglos dinámicos), sin persistencia | Superado por el Proyecto 2 |
| **Proyecto 2** | **Persistencia en archivos binarios y acceso aleatorio** | **Completo (este código)** |

> La documentación técnica completa del Proyecto 2 (estructuras, `sizeof`, diseño del detalle de la orden, firmas, orden de escrituras y preguntas de análisis) está en **[README_V2.md](README_V2.md)**.

## Qué cambió respecto al Proyecto 1

- Los datos viven en disco (`datos/*.bin`) y sobreviven entre ejecuciones. Ya no existen `SistemaTaller` ni los arreglos dinámicos: cada operación abre el archivo, calcula la posición del registro, hace `seekg`/`seekp`, lee o escribe **un solo registro** y cierra.
- Búsqueda por ID por cálculo directo: `posicion = sizeof(ArchivoHeader) + (id - 1) * sizeof(Estructura)`.
- Borrado **lógico** (`eliminado`) en todas las entidades; los registros nunca se mueven.
- Mecánicos con **baja lógica** que conserva el historial de sus órdenes.
- El detalle de cada orden se guarda dentro de su registro (arreglo fijo de hasta 20 líneas).
- Operaciones compuestas que modifican varios archivos con validación previa y un orden de escritura razonado.
- Funciones nuevas: verificación de integridad, respaldo y tres reportes.

## Compilar y ejecutar

```
g++ -std=c++11 -Wall -Wextra -o taller *.cpp
./taller            (Windows: taller.exe)
```

Al iniciar, el programa crea la carpeta `datos/` y los archivos que falten. En el primer arranque solicita los datos del taller. La carpeta `datos/` incluida viene pre-poblada (8 vehículos, 4 mecánicos —uno de baja—, 12 repuestos y 10 órdenes en los tres estados).

## Menú

```
1. Gestión de Vehículos        4. Órdenes de Servicio
2. Gestión de Mecánicos        5. Reportes
3. Inventario de Repuestos     6. Mantenimiento (integridad y backup)
0. Salir
```

## Organización del código

| Capa | Archivos |
|---|---|
| Persistencia (solo `<fstream>`, sin `cin/cout`, sin reglas de negocio) | `archivos.cpp`, `persistencia.cpp` |
| Lógica de negocio (sin `cin/cout`) | `vehiculos.cpp`, `mecanicos.cpp`, `repuestos.cpp`, `ordenes.cpp`, `reportes.cpp`, `mantenimiento.cpp`, `sistema.cpp`, `utilidades.cpp` |
| Presentación (nunca abre archivos) | `main.cpp`, `entrada.cpp`, `mensajes.cpp`, `menu_vehiculos.cpp`, `menu_mecanicos.cpp`, `menu_repuestos.cpp`, `menu_ordenes.cpp`, `menu_reportes.cpp`, `menu_mantenimiento.cpp` |
| Definiciones comunes | `taller.h` |

## Archivos de datos (`datos/`)

| Archivo | Contenido |
|---|---|
| `taller.bin` | Un único registro `Taller` (sin header) |
| `vehiculos.bin` | Vehículos (header + registros) |
| `mecanicos.bin` | Mecánicos activos y dados de baja |
| `repuestos.bin` | Inventario |
| `ordenes.bin` | Órdenes ABIERTAS, CERRADAS y ANULADAS, con su detalle |

## Reglas de negocio principales

- **Vehículos:** placa única entre activos; año 1950–2027; no se eliminan si tienen órdenes.
- **Mecánicos:** cédula única; especialidad MOTOR, FRENOS, ELECTRICIDAD, SUSPENSION o GENERAL; no se dan de baja con órdenes ABIERTAS; máximo 3 órdenes abiertas por mecánico.
- **Repuestos:** código único entre activos; alerta BAJO STOCK (`stock <= stockMinimo`); no se eliminan si aparecen en alguna orden.
- **Órdenes:** un vehículo no puede tener dos órdenes abiertas; el kilometraje de ingreso no puede ser menor al del vehículo; el precio de cada línea es el vigente al agregarla (precio histórico); anular devuelve el stock de todas las líneas.

## Pruebas

La carpeta `pruebas/` contiene las entradas de los 8 casos de uso (`entrada_1a.txt`, `entrada_3b.txt`, …) y `ejecutar_pruebas.bat`, que los ejecuta en orden, genera `salida_*.txt` y deja `datos/` pre-poblado. Detalle en [README_V2.md](README_V2.md), sección 9.

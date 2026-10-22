# Sistema de Gestión de Taller Mecánico (SistemaTaller)

Proyecto desarrollado para la materia de Programación 2 en la **Universidad Rafael Urdaneta (URU)**.

## 👥 Integrantes
- Jhogerths Shiloung Chiu Herrera
- Samuel Elías Sánchez Hernández

---

## 🛠️ Descripción del Sistema
Sistema modular en C++ diseñado para la gestión integral de un taller automotriz. El proyecto implementa una arquitectura estructurada en capas (separación entre lógica de negocio y menús de presentación) e integra gestión de memoria dinámica mediante arreglos redimensionables con un factor de crecimiento automático de 2x (`capacity doubling`).

---

## 🚀 Evolución del Proyecto

### 📌 Commit 1 (Base del Sistema)
- Definición de la estructura de datos base para `Vehiculo`.
- Implementación del registro y listado básico de vehículos.
- Asignación inicial de memoria dinámica.

### 🌟 Commit 2 (Modularización y Nuevos Módulos)
- **Cabecera Unificada (`taller.h`)**: Definición del contenedor global `SistemaTaller` que unifica vehículos, mecánicos, repuestos y órdenes de servicio.
- **Módulo de Vehículos (`vehiculos.cpp`)**: Validación robusta de datos (año, kilometraje) y control de registros duplicados por placa.
- **Módulo de Mecánicos (`mecanicos.cpp`)**: Registro de datos del personal técnico, asignación de especialidades y control de órdenes activas.
- **Módulo de Repuestos (`repuestos.cpp`)**: Control de inventario y precios con sistema de alertas de bajo stock.
- **Manejo Seguro de Entradas**: Implementación de limpieza de búfer de entrada para prevenir bucles infinitos.

### ⚡ Commit 3 (Módulo de Órdenes de Servicio e Interfaz Visual)
- **Módulo de Órdenes (`ordenes.cpp`)**: Implementación del flujo completo de órdenes (apertura, adición de repuestos con descuento en inventario, cierre con cálculo automático de mano de obra/totales y anulación con devolución de stock).
- **Redimensionamiento Dinámico**: Gestión de detalles de repuestos por orden con duplicación de capacidad en memoria dinámica.
- **Interfaz y Reporte Tabulado**: Submenú de 8 opciones y cuadro de salida tabulado para detalles de órdenes.

---

## 📂 Estructura de Archivos

```text
├── taller.h          # Cabecera central con estructuras (Vehiculo, Mecanico, Repuesto, OrdenServicio, SistemaTaller) y prototipos
├── vehiculos.cpp     # Lógica de negocio y menús interactivos de vehículos
├── mecanicos.cpp     # Lógica de negocio y listado del personal mecánico
├── repuestos.cpp     # Lógica de inventario, registro y alertas de stock
├── ordenes.cpp       # Lógica de órdenes de servicio, submenú y reportes tabulados
├── main.cpp          # Menú principal e inicialización/liberación del sistema
└── README.md         # Documentación general del proyecto

🚀 Instrucciones de Compilación y Ejecución
Para compilar el proyecto completo con todos sus módulos desde la terminal, ejecuta el siguiente comando:

```bash
g++ main.cpp ordenes.cpp vehiculos.cpp mecanicos.cpp repuestos.cpp -o taller

Configurar la consola para soporte UTF-8 (evita caracteres rotos):

```bash
chcp 65001

Ejecutar el programa:

```bash
.\taller.exe
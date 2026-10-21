# Sistema de Gestión de Taller Mecánico (SistemaTaller)

Proyecto desarrollado para la materia de Programación 2 en la **Universidad Rafael Urdaneta (URU)**[cite: 1].

## 👥 Integrantes
- Jhogerths Shiloung Chiu Herrera[cite: 1]
- Samuel Elías Sánchez Herández[cite: 1]

---

## 🛠️ Descripción del Sistema
Sistema modular en C++ diseñado para la gestión integral de un taller automotriz. El proyecto implementa una arquitectura estructurada en capas (separación entre lógica de negocio y menú de presentación) e integra gestión de memoria dinámica mediante arreglos redimensionables con un factor de crecimiento automático de 2x (`capacity doubling`).

---

## 🚀 Evolución del Proyecto

### 📌 Commit 1 (Base del Sistema)
- Definición de la estructura de datos base para `Vehiculo`[cite: 2].
- Implementación del registro y listado básico de vehículos[cite: 2].
- Asignación inicial de memoria dinámica.

### 🌟 Commit 2 (Modularización y Nuevos Módulos)
- **Cabecera Unificada (`taller.h`)**: Definición del contenedor global `SistemaTaller` que unifica vehículos, mecánicos, repuestos y órdenes de servicio[cite: 2].
- **Módulo de Vehículos (`vehiculos.cpp`)**: Validación robusta de datos (año, kilometraje) y control de registros duplicados por placa[cite: 2, 3].
- **Módulo de Mecánicos (`mecanicos.cpp`)**: Registro de datos del personal técnico, asignación de especialidades y control de órdenes activas[cite: 2, 3].
- **Módulo de Repuestos (`repuestos.cpp`)**: Control de inventario y precios con **sistema de alertas de bajo stock** ($\le 3$ unidades)[cite: 2, 3].
- **Manejo Seguro de Entradas**: Implementación de `<limits>` para limpiar el búfer de entrada y prevenir bucles infinitos ante datos inválidos.

---

## 📂 Estructura de Archivos

```text
├── taller.h          # Cabecera central con estructuras (Vehiculo, Mecanico, Repuesto, SistemaTaller) y prototipos
├── vehiculos.cpp     # Lógica de negocio y menús interactivos de vehículos
├── mecanicos.cpp     # Lógica de negocio y listado del personal mecánico
├── repuestos.cpp     # Lógica de inventario, registro y alertas de stock
├── main.cpp          # Menú principal e inicialización/liberación del sistema
└── README.md         # Documentación general del proyecto

--

## 🚀 Instrucciones de Compilación y Ejecución

Para compilar el proyecto con todos sus módulos desde la terminal de VS Code, ejecuta el siguiente comando:

```bash
g++ main.cpp vehiculos.cpp mecanicos.cpp repuestos.cpp -o taller
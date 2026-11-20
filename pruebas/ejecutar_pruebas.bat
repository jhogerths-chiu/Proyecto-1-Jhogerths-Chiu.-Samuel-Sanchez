@echo off
rem ==========================================================================
rem  Ejecuta los casos de uso del Proyecto 2 y genera pruebas\salida_*.txt
rem  Uso:  compilar primero (ver README_V2.md) y luego, desde la carpeta del
rem        proyecto:   pruebas\ejecutar_pruebas.bat
rem  AL FINAL deja datos\ pre-poblado (8 vehiculos, 4 mecanicos, 12 repuestos,
rem  10 ordenes) listo para entregar. ATENCION: borra la carpeta datos\ actual.
rem ==========================================================================
setlocal enabledelayedexpansion
cd /d "%~dp0.."
if not exist taller.exe ( echo Falta taller.exe. Compile primero. & exit /b 1 )

echo Limpiando datos y respaldos anteriores...
if exist datos rmdir /s /q datos
for /d %%d in (backup_*) do rmdir /s /q "%%d"

echo Caso 1: persistencia entre ejecuciones (base vacia)
taller.exe < pruebas\entrada_1a.txt > pruebas\salida_1a.txt
taller.exe < pruebas\entrada_1b.txt > pruebas\salida_1b.txt

echo Caso 2: acceso directo por ID (base vacia)
rmdir /s /q datos
taller.exe < pruebas\entrada_2.txt > pruebas\salida_2.txt

echo Poblando datos de prueba...
rmdir /s /q datos
taller.exe < pruebas\entrada_0_poblar.txt > pruebas\salida_0_poblar.txt

echo Caso 3: la orden sobrevive con su detalle
taller.exe < pruebas\entrada_3a.txt > pruebas\salida_3a.txt
taller.exe < pruebas\entrada_3b.txt > pruebas\salida_3b.txt
taller.exe < pruebas\entrada_3c.txt > pruebas\salida_3c.txt

echo Caso 4: orden con 15 lineas
taller.exe < pruebas\entrada_4a.txt > pruebas\salida_4a.txt
taller.exe < pruebas\entrada_4b.txt > pruebas\salida_4b.txt

echo Caso 5: anulacion con reversion en disco
taller.exe < pruebas\entrada_5a.txt > pruebas\salida_5a.txt
taller.exe < pruebas\entrada_5b.txt > pruebas\salida_5b.txt

echo Caso 6: baja de mecanico e historial
taller.exe < pruebas\entrada_6.txt > pruebas\salida_6.txt

echo Caso 8: reportes
taller.exe < pruebas\entrada_8.txt > pruebas\salida_8.txt

echo Caso 7: integridad tras una restauracion parcial
taller.exe < pruebas\entrada_7a.txt > pruebas\salida_7a.txt
set B=
for /f "delims=" %%d in ('dir /b /ad /o:n backup_*') do set B=%%d
echo Restaurando solo vehiculos.bin desde !B! ...
copy /y "!B!\vehiculos.bin" datos\vehiculos.bin >nul
taller.exe < pruebas\entrada_7b.txt > pruebas\salida_7b.txt

echo Dejando datos\ pre-poblado para la entrega...
rmdir /s /q datos
for /d %%d in (backup_*) do rmdir /s /q "%%d"
taller.exe < pruebas\entrada_0_poblar.txt > pruebas\salida_0_poblar.txt

echo Listo. Salidas en pruebas\salida_*.txt
endlocal

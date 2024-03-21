
# CONTROLVIT

Este proyecto consiste en monitorizar los varidores de frecuencia y poder acceder a su estado de manera remota


## Caracteristicas

- Monitorizacion Variador de frecuencia
- Calcular el ahorro de CO2 (solo en modelos fotovoltaicos)
- API
- ALIMENTACION 5V DC
- ALIMENTACION 230V AC
- DISPLAY CON INFORMACION DEL EQUIPO
- COMUNICACION MODBUS RTU
- ASISTENTE PARA CONFIGURAR WIFI 


## FAQ

#### ¿Cual es la contraseña de al wifi CONTROLVIT?

La contraseña es 123456789

#### Como puedo sacar los datos del variador para gestionarlos en mi propio sistema

Tenemos una peticion HTTP GET que nos retorna un JSON con todos los datos del variador. La URL es IP/api


## Configuracion hardware

Conexion con el variador
```bash
  A-> 485+
```
```bash
  B-> 485-
```
```bash
  P14-00-> 3
```
```bash
  P14.01-> 9600
```
```bash
  P14.02-> 0
```
```bash
  Jumper 4-> ON
```
![Untitled](https://github.com/agente47m/controlvit/assets/14801079/7a169059-c211-4540-a094-88e79c98f2a4)

## Parametros Modbus
Para enviar los datos modbus las direcciones de los comandos se tiene que pasar a decimal 

Por ejemplo 

**P05.06**

05 → 05(hexadecimal)

06→ 06 (hexadecimal )

la dirección sería **0506**

**P10.01**

10→ 0A

01→ 01 

LA DIRECCIÓN ES **0A01**

## Parametros Modbus

Hay que preconfigurar el variador

P00.01→”canal de comando de operación mediante comunicación”= 2

P00.02 →”canal comunicación modbus”

P00.06→ 8

## Comandos Modbus
- Estado del variador
    - 1 = marcha hacia adelante
    - 2= marcha sentido inverso
    - 3= Paro
    - 4= Fallo
    - 5= estado POFF
    - 6= Estado de preexcitacion



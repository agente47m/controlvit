
# CONTROLVIT

Este proyecto consiste en monitorizar los varidores de frecuencia y poder acceder a su estado de manera remota


## Caracteristicas

- Monitorizacion Variador de frecuencia
- Calcular el ahorro de CO2 (solo en modelos fotovoltaicos)
- API


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


# Exam_Rank_06
# mini_serv

## Descripción

Este proyecto consiste en la implementación de un servidor en C que escucha conexiones de clientes en un puerto específico en `127.0.0.1`, permitiendo que los clientes se comuniquen entre sí. El programa asigna a cada cliente un identificador único y retransmite los mensajes recibidos de un cliente a todos los demás clientes conectados. Además, gestiona correctamente la llegada y salida de los clientes, notificando a los demás usuarios cuando un cliente se conecta o se desconecta.

## Funcionalidad del programa

El programa debe cumplir con los siguientes requisitos:

- Debe escuchar conexiones en la dirección `127.0.0.1` y en el puerto especificado como primer argumento.
- Si no se proporciona un argumento, debe escribir en `stderr` el mensaje `"Wrong number of arguments\n"` y salir con un código de estado 1.
- Si una llamada al sistema falla antes de que el servidor empiece a aceptar conexiones, debe escribir en `stderr` el mensaje `"Fatal error\n"` y salir con un código de estado 1.
- Si no se puede asignar memoria, debe escribir en `stderr` el mensaje `"Fatal error\n"` y salir con un código de estado 1.
- El programa debe ser no bloqueante, lo que significa que debe usar `select` para gestionar múltiples descriptores de archivo al mismo tiempo.
- Los clientes conectados deben poder enviar y recibir mensajes, pero no deben ser desconectados si no leen los mensajes.

### Conexión de clientes

- Al conectarse, se le asigna al cliente un identificador (id) único. El primer cliente recibe el id `0`, el segundo cliente recibe el id `1`, y así sucesivamente.
- Cuando un cliente se conecta, el servidor debe notificar a todos los demás clientes con el siguiente mensaje: `"server: client %d just arrived\n"`.

### Envío y recepción de mensajes

- Los mensajes que envíen los clientes solo contendrán caracteres imprimibles, y no es necesario validarlos.
- Un único mensaje puede contener múltiples líneas (`\n`).
- Cuando el servidor recibe un mensaje de un cliente, debe reenviar el mensaje a todos los demás clientes con el formato: `"client %d: "` antes de cada línea.

### Desconexión de clientes

- Cuando un cliente se desconecta, el servidor debe notificar a todos los demás clientes con el mensaje: `"server: client %d just left\n"`.

## Funciones permitidas

El proyecto solo permite el uso de un conjunto limitado de funciones de las siguientes bibliotecas:

- **<unistd.h>**: `write`, `close`, `select`
- **<sys/socket.h>**: `socket`, `accept`, `listen`, `send`, `recv`, `bind`
- **<string.h>**: `strstr`, `strlen`, `strcpy`, `strcat`, `memset`, `bzero`
- **<stdlib.h>**: `malloc`, `realloc`, `free`, `calloc`, `atoi`, `exit`
- **<stdio.h>**: `sprintf`

## Reglas adicionales

- El programa no debe contener macros `#define`.
- Debe ejecutarse únicamente en la dirección `127.0.0.1`.
- Debes asegurarte de que no haya fugas de memoria ni de descriptores de archivo.

## Uso

Compila el programa usando un compilador C, y luego ejecútalo pasando como argumento el puerto en el que el servidor escuchará:

```bash
gcc mini_serv.c -o mini_serv
./mini_serv <puerto>

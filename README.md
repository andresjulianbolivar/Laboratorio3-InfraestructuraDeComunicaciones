# Cómo ejecutar el código del broker TCP

## Compilación
Primero, compila cada archivo usando un compilador de C compatible con Windows:

```bash
gcc broker.c -o broker.exe -lws2_32
gcc publicador.c -o publicador.exe -lws2_32
gcc suscriptor.c -o suscriptor.exe -lws2_32
```

## Ejecución

### 1. Iniciar el Broker
Abre una terminal, ubícate en el directorio donde esté el ejecutable broker.exe y ejecuta:
```bash
broker.exe <puerto>
```
Ejemplo:
```bash
broker.exe 5555
```

### 2. Iniciar un Suscriptor
Abre otra terminal nueva, ubícate en el directorio donde esté el ejecutable suscriptor.exe y ejecuta:
```bash
suscriptor.exe <ip_broker> <puerto_broker> <tema>
```
Ejemplo:
```bash
suscriptor.exe 127.0.0.1 5555 partido1
```

### 3. Iniciar un Publicador
Abre otra terminal nueva, ubícate en el directorio donde esté el ejecutable publicador.exe y ejecuta:
```bash
publicador.exe <ip_broker> <puerto_broker> <tema>
```
Ejemplo:
```bash
publicador.exe 127.0.0.1 5555 partido1
```

### Orden de ejecución
1. Primero ejecuta el broker
2. Después ejecuta los suscriptores
3. Finalmente ejecuta los publicadores
4. Escribe mensajes en la terminal del publicador para que aparezcan en los suscriptores


# Como ejecutar el código del broker UDP

## Compilación
Primero, compila cada archivo usando un compilador de C compatible con Windows:

```bash
gcc broker_udp.c -o broker_udp.exe -lws2_32
gcc publisher_udp.c -o publisher_udp.exe -lws2_32
gcc subscriber_udp.c -o subscriber_udp.exe -lws2_32
```

## Ejecución

### 1. Iniciar el Broker
Abre una terminal y ejecuta:
```bash
./broker_udp.exe <puerto>
```
Ejemplo:
```bash
./broker_udp.exe 8080
```

### 2. Iniciar un Suscriptor
Abre otra terminal nueva y ejecuta:
```bash
./subscriber_udp.exe <ip_broker> <puerto_broker> <tema>
```
Ejemplo:
```bash
./subscriber_udp.exe 127.0.0.1 8080 noticias
```

### 3. Iniciar un Publicador
Abre otra terminal nueva y ejecuta:
```bash
./publisher_udp.exe <ip_broker> <puerto_broker> <tema>
```
Ejemplo:
```bash
./publisher_udp.exe 127.0.0.1 8080 noticias
```

### Orden de ejecución
1. Primero ejecuta el broker
2. Después ejecuta los suscriptores
3. Finalmente ejecuta los publicadores
4. Escribe mensajes en la terminal del publicador para que aparezcan en los suscriptores

# Librerías usadas

## stdio.h
Librería estándar de entrada y salida de C. Proporciona funciones para imprimir texto en pantalla y leer datos del teclado.

**Funciones utilizadas:**
- `printf()`: Imprime texto formateado en la consola
- `fgets()`: Lee una línea completa desde el teclado
- `sprintf()`: Formatea texto y lo guarda en un string
- `fflush()`: Fuerza la escritura inmediata del texto en pantalla
- `sscanf()`: Extrae y convierte datos de un string

## stdlib.h
Librería estándar de C que contiene funciones de propósito general del sistema.

**Funciones utilizadas:**
- `atoi()`: Convierte un string de números a un entero
- Manejo de memoria y utilidades del sistema

## string.h
Librería para manipulación y procesamiento de cadenas de texto (strings).

**Funciones utilizadas:**
- `strlen()`: Calcula la longitud de un string
- `strcmp()`: Compara si dos strings son iguales
- `strncmp()`: Compara los primeros N caracteres de dos strings
- `strcpy()`: Copia un string a otro
- `strcspn()`: Encuentra la posición de un carácter específico en un string

## winsock2.h
Librería específica de Windows para programación de redes y sockets. Permite la comunicación entre computadoras a través de internet o redes locales.

**Funciones y estructuras utilizadas:**
- `WSAStartup()`: Inicializa la librería de sockets de Windows
- `WSACleanup()`: Libera los recursos de la librería de sockets
- `socket()`: Crea un nuevo socket para comunicación de red
- `bind()`: Asocia un socket con una dirección IP y puerto específicos
- `sendto()`: Envía datos UDP a una dirección específica
- `recvfrom()`: Recibe datos UDP y captura la dirección del remitente
- `closesocket()`: Cierra un socket y libera sus recursos
- `struct sockaddr_in`: Estructura que almacena direcciones IP y puertos
- `htons()`: Convierte números al formato de red
- `ntohs()`: Convierte números del formato de red al formato local
- `inet_addr()`: Convierte direcciones IP de texto a formato binario
- `inet_ntoa()`: Convierte direcciones IP de formato binario a texto

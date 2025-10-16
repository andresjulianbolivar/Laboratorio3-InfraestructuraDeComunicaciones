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


# Cómo ejecutar el código del broker UDP

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

## windows.h
Librería principal del sistema operativo Windows, que proporciona acceso directo a la API de Windows (Windows API). Permite controlar aspectos del sistema como procesos, hilos, sincronización, memoria, manejo de archivos y otros recursos del sistema operativo.

**Funciones y estructuras utilizadas:**
- `CreateThread()`: Crea un nuevo hilo (thread) dentro del proceso actual. En el programa TCP, se usa en el broker para atender múltiples clientes de forma concurrente, de modo que cada conexión (suscriptor o publicador) es gestionada en un hilo independiente.

- `DWORD WINAPI`: Especifica el tipo de función que se utiliza como punto de entrada de un hilo. Indica que la función devuelve un valor DWORD (entero sin signo de 32 bits) y utiliza el convenio de llamada WINAPI necesario para que el sistema operativo Windows pueda ejecutarla correctamente como hilo.

- `LPVOID`: Es un puntero genérico a cualquier tipo de dato (void* en C estándar). Se usa como parámetro en funciones de hilos para pasar información al hilo recién creado, como por ejemplo el descriptor del socket asociado al cliente.

- `CRITICAL_SECTION`: Estructura utilizada para implementar secciones críticas, que son mecanismos de sincronización de hilos. Permite asegurar que solo un hilo acceda a una parte del código o recurso compartido a la vez, evitando condiciones de carrera (race conditions). En el programa, se usa para proteger el acceso a la lista global de suscriptores (subscribers).

- `InitializeCriticalSection()`: Inicializa una estructura CRITICAL_SECTION antes de usarla. Se llama una vez al inicio del programa (en el main) para preparar el mecanismo de bloqueo.

- `EnterCriticalSection()`:Permite que un hilo entre a una sección crítica. Si otro hilo ya está dentro, el hilo actual espera hasta que el recurso esté disponible. En el broker, se usa para proteger el acceso a la lista de suscriptores durante operaciones de alta (add_subscriber()) o envío de mensajes (publish_message()).

- `LeaveCriticalSection()`: Libera la sección crítica, permitiendo que otros hilos puedan entrar. Se usa inmediatamente después de completar una operación sobre la lista compartida.

- `DeleteCriticalSection()`: Libera los recursos asociados con una sección crítica una vez que ya no se necesita, generalmente antes de finalizar el programa.

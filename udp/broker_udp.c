/*
 * BROKER UDP - Sistema Publish/Subscribe
 * 
 * 1) FUNCIONAMIENTO:
 * Este código implementa un broker UDP que actúa como intermediario en un sistema
 * de mensajería publish/subscribe. El broker:
 * - Recibe suscripciones de clientes a temas específicos
 * - Recibe mensajes publicados por otros clientes
 * - Reenvía los mensajes a todos los suscriptores del tema correspondiente
 * 
 * El broker escucha en un puerto UDP y maneja dos tipos de mensajes:
 * - "SUBSCRIBE <tema>": Registra un cliente como suscriptor de un tema
 * - "PUBLISH <tema> <mensaje>": Publica un mensaje en un tema específico
 */

// Librerías del sistema
#include <stdio.h>     // Para funciones de entrada/salida como printf()
#include <stdlib.h>    // Para funciones generales como atoi()
#include <string.h>    // Para manipulación de cadenas como strcmp(), strcpy()
#include <winsock2.h>  // Para programación de sockets en Windows

#define MAX_SUBS 100   // Máximo número de suscriptores permitidos

// Estructura para almacenar información de cada suscriptor
typedef struct {
    struct sockaddr_in addr;  // Dirección IP y puerto del suscriptor (winsock2.h)
    char topic[64];          // Tema al que está suscrito el cliente
} Subscriber;

// Array global para almacenar todos los suscriptores registrados
Subscriber subs[MAX_SUBS];
// Contador del número actual de suscriptores
int sub_count = 0;

int main(int argc, char *argv[]) {
    // Verificar que se proporcione el puerto como argumento
    if (argc < 2) {
        printf("Uso: broker_udp <puerto>\n");  // stdio.h: Imprime mensaje de uso
        return 1;
    }

    // Inicializar Winsock para usar sockets en Windows
    WSADATA wsa;                              // winsock2.h: Estructura para datos de Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);         // winsock2.h: Inicializa librería Winsock versión 2.2

    // Crear socket UDP
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0); // winsock2.h: Crea socket IPv4 UDP
    struct sockaddr_in server, client;            // winsock2.h: Estructuras para direcciones de red
    int client_len = sizeof(client);              // Tamaño de la estructura cliente

    // Configurar dirección del servidor
    server.sin_family = AF_INET;                  // winsock2.h: Especifica familia IPv4
    server.sin_addr.s_addr = INADDR_ANY;         // winsock2.h: Acepta conexiones de cualquier IP
    server.sin_port = htons(atoi(argv[1]));      // winsock2.h: htons() convierte a network byte order
                                                 // stdlib.h: atoi() convierte string a entero

    // Asociar el socket con la dirección y puerto especificados
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        // winsock2.h: bind() asocia socket con dirección local, SOCKET_ERROR indica error
        printf("Error en bind()\n");             // stdio.h: Imprime mensaje de error
        return 1;
    }

    printf("Broker UDP escuchando en puerto %s...\n", argv[1]); // stdio.h: Confirma inicio del broker

    char buffer[1024];                       // Buffer para almacenar mensajes recibidos
    while (1) {                             // Bucle infinito para escuchar mensajes
        // Recibir mensaje UDP de cualquier cliente
        int recv_len = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
                                (struct sockaddr*)&client, &client_len);
        // winsock2.h: recvfrom() recibe datos UDP y captura dirección del remitente
        
        if (recv_len <= 0) continue;        // Ignorar si no se recibió nada
        buffer[recv_len] = '\0';            // string.h: Agregar terminador nulo al mensaje

        // Procesar comando SUBSCRIBE
        if (strncmp(buffer, "SUBSCRIBE ", 10) == 0) {
            // string.h: strncmp() compara los primeros 10 caracteres del mensaje
            if (sub_count < MAX_SUBS) {     // Verificar que no se exceda el límite de suscriptores
                // Registrar nuevo suscriptor
                strcpy(subs[sub_count].topic, buffer + 10);  // string.h: Copia el tema (después de "SUBSCRIBE ")
                subs[sub_count].addr = client;               // Guarda dirección del cliente
                sub_count++;                                 // Incrementa contador de suscriptores
                
                // Enviar confirmación al cliente
                sendto(sock, "OK SUBSCRIBED\n", 14, 0,
                       (struct sockaddr*)&client, client_len);
                // winsock2.h: sendto() envía respuesta UDP al cliente específico
                
                // Registrar la suscripción en consola
                printf("Nuevo sub: %s:%d -> %s\n",
                       inet_ntoa(client.sin_addr), ntohs(client.sin_port), buffer+10);
                // winsock2.h: inet_ntoa() convierte IP a string, ntohs() convierte puerto a host byte order
            }
        }
        // Procesar comando PUBLISH
        else if (strncmp(buffer, "PUBLISH ", 8) == 0) {
            // string.h: strncmp() compara los primeros 8 caracteres del mensaje
            char topic[64], msg[512];        // Variables para extraer tema y mensaje
            
            // Extraer tema y mensaje del comando PUBLISH
            sscanf(buffer + 8, "%s %[^\n]", topic, msg);
            // stdio.h: sscanf() parsea el string: primer %s obtiene el tema, 
            // %[^\n] obtiene todo hasta el final de línea como mensaje

            // Reenviar mensaje a todos los suscriptores del tema
            for (int i = 0; i < sub_count; i++) {
                if (strcmp(subs[i].topic, topic) == 0) {
                    // string.h: strcmp() compara si el tema del suscriptor coincide
                    sendto(sock, msg, strlen(msg), 0,
                           (struct sockaddr*)&subs[i].addr, sizeof(subs[i].addr));
                    // winsock2.h: sendto() envía mensaje a cada suscriptor del tema
                    // string.h: strlen() obtiene longitud del mensaje
                }
            }
            
            // Confirmar publicación al emisor
            sendto(sock, "OK PUBLISHED\n", 13, 0,
                   (struct sockaddr*)&client, client_len);
            // winsock2.h: Envía confirmación al cliente que publicó
            
            printf("Mensaje reenviado a suscriptores de %s\n", topic);
            // stdio.h: Registra la operación en consola
        }
    }

    // Limpieza de recursos (este código nunca se ejecuta debido al bucle infinito)
    closesocket(sock);  // winsock2.h: Cierra el socket
    WSACleanup();       // winsock2.h: Libera recursos de Winsock
    return 0;
}


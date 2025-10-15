/*
 * SUBSCRIBER UDP - Cliente Suscriptor
 * 
 * 1) FUNCIONAMIENTO:
 * Este código implementa un cliente suscriptor que se registra para recibir mensajes
 * de un tema específico desde un broker UDP en un sistema publish/subscribe. El suscriptor:
 * - Se conecta a un broker UDP especificado por IP y puerto
 * - Envía un comando "SUBSCRIBE <tema>" para registrarse en un tema específico
 * - Escucha continuamente mensajes del broker relacionados con ese tema
 * - Muestra en pantalla todos los mensajes recibidos del tema suscrito
 * 
 * El suscriptor requiere: IP del broker, puerto del broker, y tema a suscribirse
 */

// Librerías del sistema
#include <stdio.h>     // Para funciones de entrada/salida como printf(), sprintf()
#include <stdlib.h>    // Para funciones generales como atoi()
#include <string.h>    // Para manipulación de cadenas como strlen()
#include <winsock2.h>  // Para programación de sockets en Windows

int main(int argc, char *argv[]) {
    // Verificar que se proporcionen todos los argumentos requeridos
    if (argc < 4) {
        printf("Uso: subscriber_udp <ip> <puerto> <topic>\n");  // stdio.h: Imprime mensaje de uso
        return 1;
    }

    // Inicializar Winsock para usar sockets en Windows
    WSADATA wsa;                              // winsock2.h: Estructura para datos de Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);         // winsock2.h: Inicializa librería Winsock versión 2.2

    // Crear socket UDP para comunicación con el broker
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0); // winsock2.h: Crea socket IPv4 UDP

    // Configurar estructuras de direcciones de red
    struct sockaddr_in server, from;         // winsock2.h: server = dirección del broker, from = remitente de mensajes
    int from_len = sizeof(from);             // Tamaño de la estructura del remitente
    
    // Configurar dirección del servidor broker
    server.sin_family = AF_INET;            // winsock2.h: Especifica familia IPv4
    server.sin_addr.s_addr = inet_addr(argv[1]); // winsock2.h: inet_addr() convierte IP string a formato binario
    server.sin_port = htons(atoi(argv[2])); // winsock2.h: htons() convierte puerto a network byte order
                                           // stdlib.h: atoi() convierte string de puerto a entero

    // Construir y enviar comando de suscripción al broker
    char msg[128];                          // Buffer para comando SUBSCRIBE
    sprintf(msg, "SUBSCRIBE %s", argv[3]);  // stdio.h: sprintf() formatea comando con el tema especificado
    sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server, sizeof(server));
    // winsock2.h: sendto() envía comando SUBSCRIBE al broker
    // string.h: strlen() obtiene longitud exacta del comando

    // Confirmar suscripción al usuario
    printf("Suscrito al topic '%s'. Esperando mensajes...\n", argv[3]); // stdio.h: Informa estado al usuario
    char buffer[512];                       // Buffer para almacenar mensajes recibidos del broker

    // Bucle infinito para escuchar mensajes del broker
    while (1) {
        // Recibir mensajes UDP del broker
        int len = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
                           (struct sockaddr*)&from, &from_len);
        // winsock2.h: recvfrom() recibe datos UDP y captura dirección del remitente
        // Bloquea el programa hasta recibir un mensaje
        
        if (len > 0) {                      // Verificar que se recibió algo
            buffer[len] = '\0';             // Agregar terminador nulo al mensaje recibido
            printf("[EVENTO] %s\n", buffer); // stdio.h: Mostrar mensaje recibido al usuario
        }
    }

    // Limpieza de recursos (este código nunca se ejecuta debido al bucle infinito)
    closesocket(sock);  // winsock2.h: Cierra el socket UDP
    WSACleanup();       // winsock2.h: Libera recursos de Winsock
    return 0;
}

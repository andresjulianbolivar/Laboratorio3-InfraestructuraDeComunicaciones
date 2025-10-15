/*
 * PUBLISHER UDP - Cliente Publicador
 * 
 * 1) FUNCIONAMIENTO:
 * Este código implementa un cliente publicador que envía mensajes a un broker UDP
 * en un sistema publish/subscribe. El publicador:
 * - Se conecta a un broker UDP especificado por IP y puerto
 * - Permite al usuario escribir mensajes interactivamente
 * - Envía cada mensaje al broker con el formato "PUBLISH <tema> <mensaje>"
 * - Continúa enviando mensajes hasta que el usuario termine el programa
 * 
 * El publicador requiere: IP del broker, puerto del broker, y tema a publicar
 */

// Librerías del sistema
#include <stdio.h>     // Para funciones de entrada/salida como printf(), fgets()
#include <stdlib.h>    // Para funciones generales como atoi()
#include <string.h>    // Para manipulación de cadenas como strlen(), strcspn(), sprintf()
#include <winsock2.h>  // Para programación de sockets en Windows

int main(int argc, char *argv[]) {
    // Verificar que se proporcionen todos los argumentos requeridos
    if (argc < 4) {
        printf("Uso: publisher_udp <ip> <puerto> <topic>\n");  // stdio.h: Imprime mensaje de uso
        return 1;
    }

    // Inicializar Winsock para usar sockets en Windows
    WSADATA wsa;                              // winsock2.h: Estructura para datos de Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);         // winsock2.h: Inicializa librería Winsock versión 2.2

    // Crear socket UDP para comunicación con el broker
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0); // winsock2.h: Crea socket IPv4 UDP
    
    // Configurar dirección del servidor broker
    struct sockaddr_in server;               // winsock2.h: Estructura para dirección del broker
    server.sin_family = AF_INET;            // winsock2.h: Especifica familia IPv4
    server.sin_addr.s_addr = inet_addr(argv[1]); // winsock2.h: inet_addr() convierte IP string a formato binario
    server.sin_port = htons(atoi(argv[2])); // winsock2.h: htons() convierte puerto a network byte order
                                           // stdlib.h: atoi() convierte string de puerto a entero

    // Buffers para almacenar mensajes del usuario y comando completo
    char mensaje[512];                      // Buffer para mensaje ingresado por usuario
    char buffer[1024];                      // Buffer para comando PUBLISH completo

    // Mostrar información de inicio al usuario
    printf("Publicador iniciado para el tópico '%s'.\n", argv[3]); // stdio.h: Confirma inicio del publicador
    printf("Escriba mensajes (CTRL+C para salir)\n");              // stdio.h: Instrucciones para el usuario

    // Bucle principal para capturar y enviar mensajes
    while (1) {
        printf(">> ");                      // stdio.h: Muestra prompt para entrada del usuario
        fflush(stdout);                     // stdio.h: Fuerza escritura inmediata del prompt

        // Leer mensaje del usuario desde teclado
        if (!fgets(mensaje, sizeof(mensaje), stdin)) break;
        // stdio.h: fgets() lee línea completa desde entrada estándar, retorna NULL si falla
        
        mensaje[strcspn(mensaje, "\n")] = '\0';  // string.h: strcspn() encuentra posición del '\n' y lo remueve
        if (strlen(mensaje) == 0) continue;      // string.h: strlen() verifica si mensaje está vacío

        // Construir comando PUBLISH con formato: "PUBLISH <tema> <mensaje>"
        sprintf(buffer, "PUBLISH %s %s", argv[3], mensaje);
        // stdio.h: sprintf() formatea string combinando tema (argv[3]) y mensaje del usuario
        
        // Enviar comando al broker UDP
        sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr*)&server, sizeof(server));
        // winsock2.h: sendto() envía datos UDP al broker especificado
        // string.h: strlen() obtiene longitud exacta del comando a enviar
    }

    // Limpieza de recursos al finalizar el programa
    closesocket(sock);  // winsock2.h: Cierra el socket UDP
    WSACleanup();       // winsock2.h: Libera recursos de Winsock
    return 0;
}

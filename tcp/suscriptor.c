#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: subscriber <ip> <puerto> <topic>\n");
        return 1;
    }

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Error conectando con el broker.\n");
        return 1;
    }

    char cmd[128];
    sprintf(cmd, "SUBSCRIBE %s\n", argv[3]);
    send(sock, cmd, strlen(cmd), 0);

    printf("Suscrito al tópico '%s'. Esperando mensajes...\n", argv[3]);

    char buffer[4096];
    int len;
    char *ptr;
    while ((len = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[len] = '\0';

        // 🔹 Ignorar mensajes vacíos o sólo con saltos de línea
        if (len == 0 || buffer[0] == '\n' || buffer[0] == '\r' || strlen(buffer) < 5) {
            continue;
        }

        // 🔹 Buscar el salto de línea que separa encabezado y cuerpo
        char *ptr = strchr(buffer, '\n');
        if (ptr) {
            ptr++; // Avanza al cuerpo del mensaje
            if (*ptr && strlen(ptr) > 0) {
                printf("[Evento] %s\n", ptr);
            }
        }
    }


    closesocket(sock);
    WSACleanup();
    return 0;
}

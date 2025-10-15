#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: publisher <ip> <puerto> <topic>\n");
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

    printf("Conectado al broker en %s:%s\n", argv[1], argv[2]);
    printf("Publicador iniciado para el tópico '%s'.\n", argv[3]);
    printf("Escriba mensajes para enviar (CTRL+C para salir)\n\n");

    char mensaje[512], paquete[1024], buffer[128];

    while (1) {
        printf(">> ");
        fflush(stdout);

        if (!fgets(mensaje, sizeof(mensaje), stdin)) break;
        mensaje[strcspn(mensaje, "\n")] = '\0';
        if (strlen(mensaje) == 0) continue;

        int len = strlen(mensaje);
        sprintf(paquete, "PUBLISH %s %d\n%s", argv[3], len, mensaje);

        send(sock, paquete, strlen(paquete), 0);

        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Broker: %s\n", buffer);
        } else {
            printf("Broker desconectado.\n");
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

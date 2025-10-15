#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define MAX_SUBS 1024

typedef struct {
    SOCKET socket;
    char topic[64];
} Subscriber;

Subscriber subscribers[MAX_SUBS];
int sub_count = 0;
CRITICAL_SECTION lock;

DWORD WINAPI client_handler(LPVOID arg);

void add_subscriber(SOCKET s, const char *topic) {
    EnterCriticalSection(&lock);
    if (sub_count < MAX_SUBS) {
        subscribers[sub_count].socket = s;
        strncpy(subscribers[sub_count].topic, topic, 63);
        subscribers[sub_count].topic[63] = '\0';
        sub_count++;
    }
    LeaveCriticalSection(&lock);
}

void publish_message(const char *topic, const char *msg, int len) {
    EnterCriticalSection(&lock);
    for (int i = 0; i < sub_count; i++) {
        if (strcmp(subscribers[i].topic, topic) == 0) {
            char paquete[2048];
            int total = sprintf(paquete, "EVENT %s %d\n%s", topic, len, msg);
            send(subscribers[i].socket, paquete, total, 0);
        }
    }
    LeaveCriticalSection(&lock);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: broker <puerto>\n");
        return 1;
    }

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        printf("Error creando socket\n");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Error en bind\n");
        closesocket(server);
        WSACleanup();
        return 1;
    }

    listen(server, 10);
    InitializeCriticalSection(&lock);

    printf("Broker escuchando en puerto %s...\n", argv[1]);

    while (1) {
        struct sockaddr_in client;
        int c = sizeof(client);
        SOCKET new_socket = accept(server, (struct sockaddr *)&client, &c);
        CreateThread(NULL, 0, client_handler, (LPVOID)new_socket, 0, NULL);
    }

    DeleteCriticalSection(&lock);
    closesocket(server);
    WSACleanup();
    return 0;
}

DWORD WINAPI client_handler(LPVOID arg) {
    SOCKET s = (SOCKET)arg;
    char buffer[1024];
    int len;

    while ((len = recv(s, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[len] = '\0';

        if (strncmp(buffer, "SUBSCRIBE ", 10) == 0) {
            char topic[64];
            sscanf(buffer + 10, "%s", topic);
            add_subscriber(s, topic);
            send(s, "OK SUBSCRIBED\n", 14, 0);
        } else if (strncmp(buffer, "PUBLISH ", 8) == 0) {
            char topic[64];
            int msg_len;
            sscanf(buffer + 8, "%s %d\n", topic, &msg_len);
            char *msg_start = strchr(buffer, '\n');
            if (msg_start && msg_len > 0) {
                msg_start++;
                publish_message(topic, msg_start, msg_len);
                send(s, "OK PUBLISHED\n", 13, 0);
            }
        } else {
            send(s, "ERR UNKNOWN\n", 12, 0);
        }
    }

    closesocket(s);
    return 0;
}
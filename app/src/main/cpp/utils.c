#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "utils.h"

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int create_sock(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Creating socket error");
        return 0;
    }

    return sock;
}

int connect_to_server(const char *server, int port) {
    int client = create_sock();
    if (!client) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }
    struct hostent* d_addr = gethostbyname(server);
    if (d_addr == NULL) {
        perror("Getting host error");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = *((unsigned long*)d_addr->h_addr);
    addr.sin_port = htons(port);

    if (connect(client, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connecting to server error");
        return -1;
    }

    return client;
}

void send_command(int sock, const char *cmd) {
    send(sock, cmd, strlen(cmd), 0);
}

#ifndef UTILS_H
#define UTILS_H

int connect_to_server(const char *server, int port);
void send_command(int sock, const char *cmd);

#endif // UTILS_H

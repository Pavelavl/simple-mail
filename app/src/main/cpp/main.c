#include <stdio.h>
#include <string.h>
#include "./include/smtp.h"
#include "./include/pop3.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s send <smtp_server> <username> <password> <from> <to> <subject> <body>\n", argv[0]);
        printf("  %s receive <pop3_server> <username> <password>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        if (argc != 9) {
            printf("Usage: %s send <smtp_server> <username> <password> <from> <to> <subject> <body>\n", argv[0]);
            return 1;
        }
        smtp_send(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
    } else if (strcmp(argv[1], "receive") == 0) {
        if (argc != 5) {
            printf("Usage: %s receive <pop3_server> <username> <password>\n", argv[0]);
            return 1;
        }
        pop3_receive(argv[2], argv[3], argv[4]);
    } else {
        printf("Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

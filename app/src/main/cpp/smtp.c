#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include <android/log.h>
#include "smtp.h"

#define SMTP_PORT 25
#define LOG_TAG "SMTP_LOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void smtp_send(const char* login, const char* pass, const char *from, const char *to, const char *subject, const char *body) {
    const char* server = getenv("SMTP_SERVER");
    LOGI("Подключение к серверу SMTP %s:%d", server, SMTP_PORT);
    int sock = connect_to_server(server, SMTP_PORT);
    if (sock < 0) {
        LOGE("Не удалось подключиться к серверу SMTP");
        return;
    }

    char buffer[1024];
    recv(sock, buffer, sizeof(buffer) - 1, 0);
    LOGI("Ответ сервера: %s", buffer);

    const char* commands[] = {
            "EHLO localhost\r\n",
            "AUTH LOGIN\r\n",
            NULL, // Username placeholder
            NULL, // Password placeholder
            NULL, // MAIL FROM placeholder
            NULL, // RCPT TO placeholder
            "DATA\r\n",
            NULL, // Email content placeholder
            "QUIT\r\n"
    };

    char username_base64[512];
    char password_base64[512];
    char mail_from[256];
    char rcpt_to[256];
    char email_content[4096];

    // Кодирование логина и пароля
    snprintf(username_base64, sizeof(username_base64), "%s\r\n", login);
    snprintf(password_base64, sizeof(password_base64), "%s\r\n", pass);
    commands[2] = username_base64;
    commands[3] = password_base64;

    // Формирование команд MAIL FROM и RCPT TO
    snprintf(mail_from, sizeof(mail_from), "MAIL FROM:<%s>\r\n", from);
    snprintf(rcpt_to, sizeof(rcpt_to), "RCPT TO:<%s>\r\n", to);
    commands[4] = mail_from;
    commands[5] = rcpt_to;

    // Формирование письма с обязательными заголовками
    snprintf(email_content, sizeof(email_content),
             "From: %s\r\n"
             "To: %s\r\n"
             "Subject: %s\r\n"
             "Date: Fri, 22 Dec 2024 21:55:20 +0000\r\n"
             "MIME-Version: 1.0\r\n"
             "Content-Type: text/plain; charset=UTF-8\r\n"
             "\r\n"  // Пустая строка между заголовками и телом
             "%s\r\n"
             ".\r\n",
             from, to, subject, body);

    commands[7] = email_content;

    size_t command_count = sizeof(commands) / sizeof(commands[0]);

    // Отправка команд и логирование
    for (size_t i = 0; i < command_count; i++) {
        if (commands[i] != NULL) {
            send(sock, commands[i], strlen(commands[i]), 0);
            LOGI("Отправлена команда: %s", commands[i]);

            int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                LOGI("Ответ сервера: %s", buffer);

                // Проверка на ошибки
                if (strstr(buffer, "550") != NULL || strstr(buffer, "554") != NULL) {
                    LOGE("Ошибка сервера: %s", buffer);
                    break;
                }
            } else {
                LOGE("Ошибка при получении ответа от сервера");
                break;
            }
        }
    }

    close(sock);
    LOGI("Соединение закрыто после отправки письма.");
}

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>
#include "utils.h"
#include "pop3.h"

#define LOG_TAG "POP3_LOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define POP3_PORT 110

// Функция для установки тайм-аута на сокет
void set_socket_timeout(int sock, int seconds) {
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

// Функция для получения списка писем
EmailList pop3_receive() {
    const char* server = getenv("POP3_SERVER");
    const char* login = getenv("POP3_LOGIN");
    const char* pass = getenv("POP3_PASS");
    EmailList email_list = {NULL, 0};

    LOGI("Подключение к серверу POP3 %s:%d", server, POP3_PORT);
    int sock = connect_to_server(server, POP3_PORT);
    if (sock < 0) {
        LOGE("Не удалось подключиться к серверу POP3");
        return email_list;
    }

    // Устанавливаем тайм-ауты
    set_socket_timeout(sock, 10); // 10 секунд тайм-аута

    char buffer[1024];
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) {
        LOGE("Ошибка при чтении ответа от сервера");
        close(sock);
        return email_list;
    }
    buffer[bytes] = '\0';
    LOGI("Ответ сервера: %s", buffer);

    // Отправка команды USER
    char user_cmd[256];
    snprintf(user_cmd, sizeof(user_cmd), "USER %s\r\n", login);
    LOGI("Отправка команды: %s", user_cmd);
    send_command(sock, user_cmd);

    bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) {
        LOGE("Ошибка при авторизации USER");
        close(sock);
        return email_list;
    }
    buffer[bytes] = '\0';
    LOGI("Ответ USER: %s", buffer);

    // Отправка команды PASS
    char pass_cmd[256];
    snprintf(pass_cmd, sizeof(pass_cmd), "PASS %s\r\n", pass);
    LOGI("Отправка команды: PASS ******");
    send_command(sock, pass_cmd);

    bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) {
        LOGE("Ошибка при авторизации PASS");
        close(sock);
        return email_list;
    }
    buffer[bytes] = '\0';
    LOGI("Ответ PASS: %s", buffer);

    // Запрос списка писем
    send_command(sock, "LIST\r\n");
    bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) {
        LOGE("Ошибка при получении списка писем");
        close(sock);
        return email_list;
    }
    buffer[bytes] = '\0';
    LOGI("Ответ на команду LIST: %s", buffer);

    // Извлекаем список писем
    char *line = strtok(buffer, "\r\n");
    while (line != NULL) {
        email_list.count++;
        email_list.emails = realloc(email_list.emails, email_list.count * sizeof(char *));
        email_list.emails[email_list.count - 1] = strdup(line);
        LOGI("Добавлено письмо: %s", line);
        line = strtok(NULL, "\r\n");
    }

    // Отправка команды QUIT
    send_command(sock, "QUIT\r\n");
    LOGI("Команда QUIT отправлена.");

    close(sock);
    LOGI("Соединение закрыто.");
    return email_list;
}

// Функция для получения содержимого письма по индексу (без декодинга)
char* pop3_retrieve(int index) {
    const char* server = getenv("POP3_SERVER");
    const char* login = getenv("POP3_LOGIN");
    const char* pass = getenv("POP3_PASS");
    LOGI("Подключение к серверу для получения письма #%d", index);
    int sock = connect_to_server(server, POP3_PORT);
    if (sock < 0) {
        LOGE("Не удалось подключиться к серверу POP3");
        return NULL;
    }

    set_socket_timeout(sock, 10);

    // Выделяем буфер для письма
    char *buffer = (char *)malloc(32768); // 32 KB буфер
    if (!buffer) {
        LOGE("Ошибка выделения памяти под буфер");
        close(sock);
        return NULL;
    }

    char user_cmd[256];
    snprintf(user_cmd, sizeof(user_cmd), "USER %s\r\n", login);
    send_command(sock, user_cmd);

    char pass_cmd[256];
    snprintf(pass_cmd, sizeof(pass_cmd), "PASS %s\r\n", pass);
    send_command(sock, pass_cmd);

    // Отправляем команду RETR
    char retr_cmd[256];
    snprintf(retr_cmd, sizeof(retr_cmd), "RETR %d\r\n", index);
    send(sock, retr_cmd, strlen(retr_cmd), 0);

    char temp_buffer[1024];
    int total_bytes = 0;
    int bytes_received;

    // Читаем ответ по частям до терминатора ".\r\n"
    while ((bytes_received = recv(sock, temp_buffer, sizeof(temp_buffer) - 1, 0)) > 0) {
        temp_buffer[bytes_received] = '\0';

        // Добавляем в основной буфер
        if (total_bytes + bytes_received < 32768) {
            memcpy(buffer + total_bytes, temp_buffer, bytes_received);
            total_bytes += bytes_received;
        } else {
            LOGE("Переполнение буфера письма!");
            break;
        }

        // Проверяем на конец данных (.\r\n)
        if (strstr(temp_buffer, "\r\n.\r\n")) {
            break; // Найден конец данных
        }
    }

    // Добавляем завершающий символ
    buffer[total_bytes] = '\0';

    if (bytes_received < 0) {
        LOGE("Ошибка при получении содержимого письма");
        free(buffer);
        close(sock);
        return NULL;
    }

    LOGI("Получено письмо #%d, размер: %d байт", index, total_bytes);

    // Закрываем соединение
    send_command(sock, "QUIT\r\n");
    close(sock);
    LOGI("Соединение закрыто после получения письма.");

    return buffer;
}



void free_email_list(EmailList *email_list) {
    for (size_t i = 0; i < email_list->count; i++) {
        free(email_list->emails[i]);
    }
    free(email_list->emails);
    email_list->emails = NULL;
    email_list->count = 0;
    LOGI("Память очищена.");
}

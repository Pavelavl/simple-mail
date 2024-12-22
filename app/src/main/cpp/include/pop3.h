#ifndef POP3_H
#define POP3_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char **emails;  // Динамический массив строк с письмами
    size_t count;   // Количество писем
} EmailList;

EmailList pop3_receive();
char* pop3_retrieve(int index);
void free_email_list(EmailList *email_list);

#ifdef __cplusplus
}
#endif

#endif // POP3_H

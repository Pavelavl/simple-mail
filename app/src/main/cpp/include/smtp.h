#ifndef SMTP_H
#define SMTP_H

#ifdef __cplusplus
extern "C" {
#endif

void smtp_send(const char* login, const char* pass, const char *from, const char *to, const char *subject, const char *body);

#ifdef __cplusplus
}
#endif

#endif // SMTP_H

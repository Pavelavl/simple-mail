#include <jni.h>
#include <string>
#include <android/log.h>
#include "./include/pop3.h"
#include "./include/smtp.h"

#define LOG_TAG "POP3_LOG"  // Тег, по которому будут отображаться логи
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT void JNICALL
Java_com_pavelavl_simple_1mail_MainActivity_setEnvVariable(
        JNIEnv *env,
        jobject thiz,
        jstring key,
        jstring value) {

    // Исправлено использование '.' вместо '->'
    const char *nativeKey = env->GetStringUTFChars(key, nullptr);
    const char *nativeValue = env->GetStringUTFChars(value, nullptr);

    if (setenv(nativeKey, nativeValue, 1) == 0) {
        LOGI("Успешно установлена переменная среды: %s = %s", nativeKey, nativeValue);
    } else {
        LOGE("Ошибка при установке переменной среды: %s", nativeKey);
    }

    // Освобождаем строки
    env->ReleaseStringUTFChars(key, nativeKey);
    env->ReleaseStringUTFChars(value, nativeValue);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_pavelavl_simple_1mail_MainActivity_pop3Retrieve(JNIEnv *env, jobject thiz, jint index) {
    char *message = pop3_retrieve(index);
    jstring result = env->NewStringUTF(message);
    free(message);
    return result;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_pavelavl_simple_1mail_MainActivity_pop3Receive(
        JNIEnv *env,
        jobject thiz) {

    LOGI("Начало выполнения pop3Receive()");  // Лог начала выполнения

    // Вызов функции для получения писем
    EmailList email_list = pop3_receive();

    // Проверка количества писем
    LOGI("Получено %zu писем", email_list.count);

    // Создание массива строк Java (jobjectArray)
    jclass string_class = env->FindClass("java/lang/String");
    jobjectArray result = env->NewObjectArray(email_list.count, string_class, nullptr);

    if (result == nullptr) {
        LOGE("Ошибка создания массива строк в Java");
        free_email_list(&email_list);
        return nullptr;
    }

    // Заполнение массива Java строками из email_list
    for (size_t i = 0; i < email_list.count; i++) {
        LOGI("Добавление письма %zu: %s", i + 1, email_list.emails[i]);

        jstring email = env->NewStringUTF(email_list.emails[i]);
        if (email == nullptr) {
            LOGE("Ошибка создания строки Java для письма %zu", i + 1);
            continue;
        }
        env->SetObjectArrayElement(result, i, email);
        env->DeleteLocalRef(email);  // Удаляем локальную ссылку
    }

    // Освобождение памяти, выделенной для email_list
    free_email_list(&email_list);
    LOGI("Освобождение памяти завершено");

    // Возвращаем массив строк в Java
    LOGI("Завершение pop3Receive()");
    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pavelavl_simple_1mail_MainActivity_smtpSend(
        JNIEnv *env,
        jobject thiz,
        jstring login,
        jstring pass,
        jstring from,
        jstring to,
        jstring subject,
        jstring body) {
    const char *login_str = env->GetStringUTFChars(login, nullptr);
    const char *pass_str = env->GetStringUTFChars(pass, nullptr);
    const char *from_str = env->GetStringUTFChars(from, nullptr);
    const char *to_str = env->GetStringUTFChars(to, nullptr);
    const char *subject_str = env->GetStringUTFChars(subject, nullptr);
    const char *body_str = env->GetStringUTFChars(body, nullptr);

    // Вызов функции для отправки письма
    smtp_send(login_str, pass_str, from_str, to_str, subject_str, body_str);

    // Освобождение строковых данных
    env->ReleaseStringUTFChars(from, from_str);
    env->ReleaseStringUTFChars(to, to_str);
    env->ReleaseStringUTFChars(subject, subject_str);
    env->ReleaseStringUTFChars(body, body_str);
}

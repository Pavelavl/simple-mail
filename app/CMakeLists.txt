cmake_minimum_required(VERSION 3.10.2)

# Указываем, где находятся исходники C
project("simple_mail")

# Добавляем исходники C
add_library(
        native-lib
        SHARED
        src/main/cpp/native-lib.cpp
        src/main/cpp/pop3.c
        src/main/cpp/smtp.c
        src/main/cpp/utils.c

)

# Указываем путь к заголовочным файлам
include_directories(
        src/main/cpp/include
)

# Линкуем с необходимыми библиотеками
find_library(
        log-lib
        log
)

target_link_libraries(
        native-lib
        ${log-lib}
)

CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lssl -lcrypto
SRC = main.c smtp.c pop3.c utils.c
OBJ = $(SRC:.c=.o)
TARGET = server

all: clean $(TARGET) clean-obj

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

clean-obj:
	rm -f $(OBJ)

help:
	@echo "Доступные команды:"
	@echo "  make        - Сборка проекта"
	@echo "  make clean  - Очистка объектных файлов и исполняемого файла"

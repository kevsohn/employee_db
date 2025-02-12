CC = gcc

DB_NAME = test
TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)

run: clean default
	./$(TARGET) -f ./$(DB_NAME).db -n 
	./$(TARGET) -f ./$(DB_NAME).db -a "Tim Horton, Terry Fox Ave., 1"
	./$(TARGET) -f ./$(DB_NAME).db -a "John Vancouver, Maple Street, 12"
	./$(TARGET) -f ./$(DB_NAME).db -l

default: $(TARGET)

clean:
	rm -f obj/*
	rm -f bin/*
	rm -f ./*.db

$(TARGET): $(OBJ)
	$(CC) $? -o $@

obj/%.o: src/%.c
	$(CC) -c $< -o $@ -Iinclude

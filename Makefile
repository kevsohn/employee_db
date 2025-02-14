CC = gcc

DB_NAME = employees
TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)

run: clean default
	./$(TARGET) -f ./$(DB_NAME).db -n 
	#./$(TARGET) -f ./$(DB_NAME).db -n 
	./$(TARGET) -f ./$(DB_NAME).db -a "Tim Horton,Terry Fox Ave.,1"
	./$(TARGET) -f ./$(DB_NAME).db -a "John Vancouver,Maple Street,12" -l
	#./$(TARGET) -f ./$(DB_NAME).db -a "John Vancouver,Maple Street,12"
	./$(TARGET) -f ./$(DB_NAME).db -u "Tim Horton,1000" -l
	#./$(TARGET) -f ./$(DB_NAME).db -u "Tim,1000"
	./$(TARGET) -f ./$(DB_NAME).db -r "John Vancouver" -l
	#./$(TARGET) -f ./$(DB_NAME).db -r "Tim Horton" -l
	./$(TARGET) -f ./$(DB_NAME).db -a "John Canuck,Hockey Lane,101" -l

default: $(TARGET)

clean:
	rm -f obj/*
	rm -f bin/*
	rm -f ./*.db

$(TARGET): $(OBJ)
	$(CC) $? -o $@

obj/%.o: src/%.c
	$(CC) -c $< -o $@ -Iinclude

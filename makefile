TARGET = bin/final
DEBUG = bin/debug
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET)

default: $(TARGET)

debug: $(DEBUG)

clean:
	rm -fr obj/*.o
	rm -fr bin/*

$(TARGET): $(OBJ)
	clang -o $@ $? -std=c99 -Wall -Werror -Wextra

$(DEBUG): $(OBJ)
	clang -o $@ -g $? -std=c99 -Wall -Wextra -fsanitize=address

obj/%.o : src/%.c
	clang -c $< -o $@ -g -Iinc -std=c99

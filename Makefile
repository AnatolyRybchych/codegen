CC      := gcc
INCLUDE := -Iinclude
LIBS    := 
CARGS   := $(INCLUDE) -ggdb -Wall -Wextra -Werror -pedantic
OUT     := run

objects += main.o
objects += source.o

build: $(addprefix obj/, $(objects))
	@mkdir -p $(dir ./$(OUT))
	$(CC) $(CARGS) -o ./$(OUT) $^ $(LIBS)

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CARGS) -o $@ $^

run: build
	./$(OUT) test

gdb: build
	gdb ./$(OUT)

clean:
	rm $(OUT)
	rm -r ./obj

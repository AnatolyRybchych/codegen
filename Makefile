CC      := gcc
INCLUDE := -Iinclude
LIBS    := 
CARGS   := $(INCLUDE) -ggdb -Wall -Wextra -Werror -pedantic
OUT     := run

objects += main.o
objects += source.o
objects += expr.o
objects += eval.o
objects += str.o

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

gf: build
	gf2 ./$(OUT)

clean:
	rm $(OUT)
	rm -r ./obj

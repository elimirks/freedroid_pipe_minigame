# Compiler
CC   = gcc
OPTS = -g -std=gnu99

# Project name
PROJECT = freedroid_minigame

# Libraries
LIBS = -I/usr/include/SDL -lSDL -lSDL_image -lSDL_gfx

# GameCore Files and folders
SRCS = $(shell find src -name '*.c')
DIRS = $(shell find src -type d | sed 's/src/./g' ) 
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS))

# Targets
$(PROJECT): buildrepo $(OBJS)
	$(CC) $(OPTS) $(OBJS) $(LIBS) -o $@

obj/%.o: src/%.c
	$(CC) $(OPTS) $(LIBS) -c $< -o $@

clean:
	rm $(PROJECT) obj -Rf

buildrepo:
	mkdir -p obj
	for dir in $(DIRS); do mkdir -p obj/$$dir; done


# Compiler
CC   = gcc
OPTS = -g -std=gnu99

# Project name
PROJECT = freedroid-minigame

# Libraries
LIBS = -I/usr/include/SDL -lSDL -lSDL_image -lSDL_gfx

# GameCore Files and folders
CORE_SRCS = $(shell find src -name '*.c')
CORE_DIRS = $(shell find src -type d | sed 's/src/./g' ) 
CORE_OBJS = $(patsubst src/%.cpp,obj/%.o,$(CORE_SRCS))

# Targets
$(PROJECT): buildrepo $(CLIENT_OBJS) $(SERVER_OBJS) $(CORE_OBJS)
	$(CC) $(OPTS) $(CLIENT_OBJS) $(CORE_OBJS) $(LIBS) -o $@

# FIXME This doesn't do what it should be doing
obj/%.o: src/%.c
	$(CC) $(OPTS) -c $< -o $@

clean:
	rm $(PROJECT) obj -Rf

buildrepo:
	mkdir -p obj
	for dir in $(CLIENT_DIRS) $(SERVER_DIRS) $(CORE_DIRS); do mkdir -p obj/$$dir; done


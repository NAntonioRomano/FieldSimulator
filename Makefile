#**************************************************************************************************
#
#   raylib makefile - MODIFICADO PARA PROYECTOS EN C CON MÚLTIPLES ARCHIVOS
#
#**************************************************************************************************

# --- CONFIGURACIÓN DEL PROYECTO ---
# Cambiá el nombre de tu ejecutable final aquí
PROJECT_NAME    ?= mi_juego

# Agregá todos tus archivos fuente (.c) aquí, separados por espacios
SRCS            = main.c

# --- CONFIGURACIÓN DE RAYLIB Y COMPILADOR (No necesita cambios) ---
RAYLIB_PATH     ?= C:/raylib/raylib
COMPILER_PATH   ?= C:/raylib/w64devkit/bin
BUILD_MODE      ?= DEBUG

# --- LÓGICA DEL MAKEFILE (No tocar de aquí para abajo) ---
PLATFORM        ?= PLATFORM_DESKTOP

OBJS = $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
    PLATFORM_OS=WINDOWS
    export PATH := $(COMPILER_PATH):$(PATH)
else
    # Lógica para otros sistemas operativos...
endif

CC = gcc
CFLAGS += -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external
LDFLAGS = -L. -L$(RAYLIB_PATH)/src
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g
else
    CFLAGS += -s -O1
endif

# Regla principal: compila el proyecto
all: $(PROJECT_NAME)

# Regla de enlazado: une todos los archivos objeto (.o) para crear el ejecutable final
$(PROJECT_NAME): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LDLIBS)

# Regla de compilación: convierte cualquier archivo .c en un archivo .o
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

# Regla de limpieza
clean:
ifeq ($(PLATFORM_OS),WINDOWS)
	del *.o *.exe /s
else
	rm -f *.o $(PROJECT_NAME)
endif
	@echo "Limpieza completada."


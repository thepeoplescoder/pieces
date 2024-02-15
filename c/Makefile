# Makefile for pieces C implementation.
# Hopefully it is simple enough to describe the project structure.
#
# See https://makefiletutorial.com/ for an easy to follow tutorial on makefiles (not my website).
#
# Author:
#    Austin Cathey (thepeoplescoder)

ifeq ($(OS), Windows_NT)
	PLATFORM = Windows
else
	PLATFORM = $(shell uname -s)
endif

ifeq ($(PLATFORM), Windows)
	EXECUTABLE_FILE_NAME = pieces.exe
else
	EXECUTABLE_FILE_NAME = pieces
endif

SOURCE_DIR = ./src
DIR_OBJS   = $(SOURCE_DIR)/objects
DIR_EXES   = ./executables

PLATFORM_DIR_OBJ = $(DIR_OBJS)/$(PLATFORM)
PLATFORM_DIR_EXE = $(DIR_EXES)/$(PLATFORM)

PATH_TO_EXECUTABLE = $(PLATFORM_DIR_EXE)/$(EXECUTABLE_FILE_NAME)

OBJECTS = \
	$(PLATFORM_DIR_OBJ)/command_line_syntax.o \
	$(PLATFORM_DIR_OBJ)/command_line.o        \
	$(PLATFORM_DIR_OBJ)/main.o                \
	$(PLATFORM_DIR_OBJ)/subcommand.o          \
	$(PLATFORM_DIR_OBJ)/zstrings.o

all: $(PATH_TO_EXECUTABLE)

$(PATH_TO_EXECUTABLE): \
	$(PLATFORM_DIR_EXE) \
	$(OBJECTS)
		gcc -o $@ $(OBJECTS)

$(PLATFORM_DIR_EXE): \
	$(DIR_EXES)
		mkdir $@

$(DIR_EXES):
	mkdir $@

$(PLATFORM_DIR_OBJ)/command_line_syntax.o: \
	$(PLATFORM_DIR_OBJ)                 \
	$(SOURCE_DIR)/command_line_syntax.c \
	$(SOURCE_DIR)/command_line_syntax.h
		gcc -c $(SOURCE_DIR)/command_line_syntax.c -o $@

$(PLATFORM_DIR_OBJ)/command_line.o: \
	$(PLATFORM_DIR_OBJ)          \
	$(SOURCE_DIR)/command_line.c \
	$(SOURCE_DIR)/command_line.h
		gcc -c $(SOURCE_DIR)/command_line.c -o $@

$(PLATFORM_DIR_OBJ)/main.o: \
	$(PLATFORM_DIR_OBJ)  \
	$(SOURCE_DIR)/main.c
		gcc -c $(SOURCE_DIR)/main.c -o $@

$(PLATFORM_DIR_OBJ)/subcommand.o: \
	$(PLATFORM_DIR_OBJ)        \
	$(SOURCE_DIR)/subcommand.c \
	$(SOURCE_DIR)/subcommand.h
		gcc -c $(SOURCE_DIR)/subcommand.c -o $@

$(PLATFORM_DIR_OBJ)/zstrings.o: \
	$(PLATFORM_DIR_OBJ)      \
	$(SOURCE_DIR)/zstrings.c \
	$(SOURCE_DIR)/zstrings.h
		gcc -c $(SOURCE_DIR)/zstrings.c -o $@

$(PLATFORM_DIR_OBJ): \
	$(DIR_OBJS)
		mkdir $@

$(DIR_OBJS):
	mkdir $@

clean:
	rm -Rf $(DIR_OBJS)/
	rm -Rf $(DIR_EXES)/

install: \
	all
		$(info Install target not implemented yet.)
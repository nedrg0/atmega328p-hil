#
# Makefile to compile the real-time simulator
#

# Includes
PROTOCOL_DIR = ../common
INCLUDE_DIRS = $(PROTOCOL_DIR)
SOURCES_DIR = src

SOURCES = main.c $(SOURCES_DIR)/protocol.c

# Toolchain
GCC = gcc

#Flags
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))
CFLAGS = -O2 $(INCLUDES) 

TARGET = main

$(TARGET).o : $(TARGET).c
	$(GCC) $(CFLAGS) -o $@ $(SOURCES) 

.PHONY: run clean

clean:
	@rm $(TARGET).o

run: $(TARGET).o
	@./$^

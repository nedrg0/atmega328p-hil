#
# Makefile to compile the real-time simulator
#

# Includes
PROTOCOL_DIR = ../common
SOURCES_DIR = src

INCLUDE_DIRS = $(PROTOCOL_DIR) $(SOURCES_DIR)

SOURCES = main.c $(SOURCES_DIR)/protocol.c $(SOURCES_DIR)/serial.c  $(SOURCES_DIR)/solver.c 
					

# Toolchain
GCC = gcc

#Flags
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS)) 
CFLAGS = -O2 $(INCLUDES) -lm

TARGET = main

$(TARGET).o : $(TARGET).c
	$(GCC) $(CFLAGS) -o $@ $(SOURCES) 

.PHONY: run clean

clean:
	@rm $(TARGET).o

run: $(TARGET).o
	@./$^

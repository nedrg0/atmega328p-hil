#
# Makefile to compile the real-time simulator
#

# Toolchain
GCC = gcc

#Flags
CFLAGS = -O2 

TARGET = main

$(TARGET).o : $(TARGET).c
	$(GCC) $(CFLAGS) -o $@ $^

.PHONY: run clean

clean:
	@rm $(TARGET).o

run: $(TARGET).o
	@./$^

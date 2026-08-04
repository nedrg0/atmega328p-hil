#MCU Variables
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200
PORT = /dev/ttyUSB0


#Directories 

COMMON_DIR = ../../common
DRIVERS_DIR = drivers
PROTOCOL_DIR = ../../../common

AVR_ROOT_DIR = /usr/lib/avr
AVR_INCLUDE_DIR = $(AVR_ROOT_DIR)/include
AVR_BIN_DIR = $(AVR_ROOT_DIR)/bin
INCLUDE_DIRS = $(AVR_INCLUDE_DIR) $(COMMON_DIR) $(DRIVERS_DIR) $(PROTOCOL_DIR) 
LIB_DIRS = $(AVR_INCLUDE_DIR)


OBJ_DIR = obj


#Files
TARGET = app

SOURCES = main.c gpio.c uart.c ring_buffer.c timer.c protocol.c adc.c

OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.c=.o))

CPPCHECK_SOURCES = $(wildcard $(DRIVERS_DIR)/*.c $(COMMON_DIR)/*.c)


# Toolchain
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
CPPCHECK = cppcheck

#Flags
WFLAGS = -Wall -Wextra -Werror -Wshadow
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(WFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) -Og -g 
LFLAGS = -mmcu=$(MCU) $(addprefix -L, $(LIB_DIRS)) 
CPPCHECK_FLAGS = --quiet --enable=warning,style,performance,portability \
                  --error-exitcode=1 \
                  --inline-suppr -D__AVR_ATmega328P__ \
                  -isystem $(AVR_INCLUDE_DIR) \
                  $(addprefix -I , $(INCLUDE_DIRS)) \
                  --suppress=missingIncludeSystem \
                  --suppress=unmatchedSuppression \
                  --suppress=shadowVariable:*/avr/include/util/delay.h \
                  $(CPPCHECK_SOURCES)

#Build
$(TARGET).hex : $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

$(TARGET).elf : $(OBJECTS) 
	@mkdir -p $(dir $@)
	$(CC)  $(LFLAGS) $^ -o $@ 

## Compile
$(OBJ_DIR)/%.o : %.c
	$(info OBJECTS =  $(OBJECTS))
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

$(OBJ_DIR)/%.o : $(DRIVERS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

$(OBJ_DIR)/%.o : $(COMMON_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

.PHONY: all clean flash cppcheck hiltest

all: $(TARGET).hex

clean:
	@rm -r $(OBJ_DIR)
	@rm -r $(TARGET).elf
	@rm -r $(TARGET).hex

flash: $(TARGET).hex
	$(AVRDUDE) -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$^ 

cppcheck: 
	@$(CPPCHECK) $(CPPCHECK_FLAGS)

hiltest:
	gcc -O2  -o $(HIL_DIR)/main $(HIL_DIR)/main.c
	$(HIL_DIR)/main

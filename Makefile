#MCU Variables
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200
PORT = /dev/ttyUSB0


#Directories 

SRC_DIR = ./src
APP_DIR = $(SRC_DIR)/app
COMMON_DIR = $(SRC_DIR)/common
DRIVERS_DIR = $(SRC_DIR)/drivers

AVR_ROOT_DIR = /usr/lib/avr
AVR_INCLUDE_DIR = $(AVR_ROOT_DIR)/include
AVR_BIN_DIR = $(AVR_ROOT_DIR)/bin
INCLUDE_DIRS = $(AVR_INCLUDE_DIR) $(APP_DIR)
LIB_DIRS = $(AVR_INCLUDE_DIR)

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

#Files
TARGET = $(BIN_DIR)/blink

SOURCES = main.c led.c

OBJECT_NAMES = $(SOURCE:.c=.o)
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/led.o



# Toolchain
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
#Flags
WFLAGS = -Wall -Wextra -Werror -Wshadow
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(WFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) -Og -g 
LFLAGS = -mmcu=$(MCU) $(addprefix -L, $(LIB_DIRS)) 



#Build
$(TARGET).hex : $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

$(TARGET).elf : $(OBJECTS) 
	@mkdir -p $(dir $@)
	$(CC)  $(LFLAGS) $^ -o $@ 

## Compile
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

$(OBJ_DIR)/%.o : $(APP_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

$(OBJ_DIR)/%.o : $(DRIVERS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

$(OBJ_DIR)/%.o : $(COMMON_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

.PHONY: all clean

all: $(TARGET).hex

clean:
	@rm -r $(BUILD_DIR)

flash: $(TARGET).hex
	$(AVRDUDE) -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$^ 

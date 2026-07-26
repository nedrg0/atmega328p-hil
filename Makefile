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
INCLUDE_DIRS = $(AVR_INCLUDE_DIR) $(APP_DIR) $(COMMON_DIR) $(DRIVERS_DIR) $(SRC_DIR)
LIB_DIRS = $(AVR_INCLUDE_DIR)

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

#Files
TARGET = $(BIN_DIR)/blink

SOURCES = main.c gpio.c uart.c ring_buffer.c timer.c 

OBJECT_NAMES = $(SOURCE:.c=.o)
OBJECTS = 	$(OBJ_DIR)/main.o \
				   	$(OBJ_DIR)/gpio.o \
					$(OBJ_DIR)/uart.o \
					$(OBJ_DIR)/ring_buffer.o \
					$(OBJ_DIR)/timer.o
CPPCHECK_SOURCES = $(wildcard $(SRC_DIR)/*.c $(APP_DIR)/*.c $(DRIVERS_DIR)/*.c $(COMMON_DIR)/*.c)


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

.PHONY: all clean flash cppcheck

all: $(TARGET).hex

clean:
	@rm -r $(BUILD_DIR)

flash: $(TARGET).hex
	$(AVRDUDE) -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$^ 

cppcheck: 
	@$(CPPCHECK) $(CPPCHECK_FLAGS)
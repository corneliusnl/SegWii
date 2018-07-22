
DEFINES 			= 
#DEFINES 			+= -D_DEBUG_
#DEFINES			+= -DMEM_TEST

BUILD_DIR 			= ./build

MCU 				= atmega32

FORMAT 				= ihex

TARGET 				= segwii

# Optimization level, can be [0, 1, 2, 3, s]. 0 turns off optimization.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT 				= s
# -finline-functions

SEGWII_SRC =  main.c isr.c uart.c support.c bluetooth.c timers.c time.c adc.c i2c.c nunchuk.c motors.c debug.c gyro.c

# Compiler flag to set the C Standard level.
CSTANDARD_C89 		= c89
CSTANDARD_GNU89 	= gnu89
CSTANDARD_C99 		= c99
CSTANDARD_GNU99 	= gnu99
CSTANDARD 			= -std=$(CSTANDARD_GNU99)


TEXT_START 			= 0x00000

# ---------------------------------------------------------------------------
# Define directories, if needed.
DIRAVR 				= c:/winavr
DIRAVRBIN 			= $(DIRAVR)/bin
DIRAVRUTILS 		= $(DIRAVR)/utils/bin
DIRINC 				= .
DIRLIB 				= $(DIRAVR)/avr/lib


# Define programs and commands.
SHELL 				= sh

CC 					= avr-gcc

OBJCOPY 			= avr-objcopy
OBJDUMP 			= avr-objdump
SIZE 				= avr-size


REMOVE 				= rm -f
COPY 				= cp



# Define all object files.
OBJ					= $(SRC:.c=.o) $(ASRC:.asm=.o) 

# Define all listing files.
LST 				= $(OBJ:.o=.lst)

# Combine all necessary flags and optional flags. 
# Add target processor to flags.
ALL_CFLAGS 			= -mmcu=$(MCU) -I. $(CFLAGS)
ALL_ASFLAGS 		= -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)



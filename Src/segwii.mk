include common.mk

# List C source files here. (C dependencies are automatically generated.)
SRC = 	$(SEGWII_SRC)


# Compiler flag to set the C Standard level.
CSTANDARD_C89 = c89
CSTANDARD_GNU89 = gnu89
CSTANDARD_C99 = c99
CSTANDARD_GNU99 = gnu99
CSTANDARD = -std=$(CSTANDARD_GNU99)

# Optional compiler flags.
#  -g:        generate debugging information (for GDB, or for COFF conversion)
#  -O*:       optimization level
#  -f...:     tuning, see gcc manual and avr-libc documentation
#  -Wall...:  warning level
#  -Wa,...:   tell GCC to pass this to the assembler.
#    -ahlms:  create assembler listing
CFLAGS = $(DEFINES) -O$(OPT) \
	-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
	-Wall -Wstrict-prototypes \
	#-Wa,-ahlms=$(notdir $(<:.c=.lst))


# Optional assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-ahlms=$(notdir $(<:.asm=.lst)),-gstabs 


# Optional linker flags.
#  -Wl,...:   tell GCC to pass this to linker.
#  -Map:      create map file
#  --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(BUILD_TARGET).map,--cref,--section-start=.text=$(TEXT_START)

# Additional libraries
#
# Minimalistic printf version
LDFLAGS += -Wl,-u,vfprintf -lprintf_min -lm


# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.asm=.o) 

# Define all listing files.
LST = $(OBJ:.o=.lst)

# Combine all necessary flags and optional flags. 
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: build

build: clean elf hex
	@echo -------------------------------------------------------------------------------
	@echo Completed build of $(BUILD_TARGET)
	@echo
	$(SIZE) $(BUILD_TARGET).hex
	@echo -------------------------------------------------------------------------------
#	$(COPY) $(BUILD_TARGET).hex $(BUILD_DIR)/$(BUILD_TARGET).hex


elf: $(BUILD_TARGET).elf
hex: $(BUILD_TARGET).hex
eep: $(BUILD_TARGET).eep
lss: $(BUILD_TARGET).lss 
sym: $(BUILD_TARGET).sym



# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@


# Link: create ELF output file from object files.
.SECONDARY : $(BUILD_TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



# Target: clean project.
clean: clean_list

clean_list :
	$(REMOVE) $(BUILD_TARGET).hex
	$(REMOVE) $(BUILD_TARGET).eep
	$(REMOVE) $(BUILD_TARGET).obj
	$(REMOVE) $(BUILD_TARGET).cof
	$(REMOVE) $(BUILD_TARGET).elf
	$(REMOVE) $(BUILD_TARGET).map
	$(REMOVE) $(BUILD_TARGET).obj
	$(REMOVE) $(BUILD_TARGET).a90
	$(REMOVE) $(BUILD_TARGET).sym
	$(REMOVE) $(BUILD_TARGET).lnk
	$(REMOVE) $(BUILD_TARGET).lss
	$(REMOVE) $(OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) .dep/*


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all build elf hex eep clean clean_list



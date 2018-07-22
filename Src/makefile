include common.mk

# Default target.
all: clean segwii

build: all
	$(COPY) *.hex $(BUILD_DIR)/
	$(COPY) *.eep $(BUILD_DIR)/
	
segwii:	segwii_fw

debug: segwii_fw_debug

program: 
#	avrdude -p m32 -P /dev/tty.usbmodemfd121 -v -c stk500v2 -e -V -U flash:w:segwii.hex
#	avrdude -p m32 -P /dev/ttyACM0 -v -c stk500v2 -e -V -U flash:w:segwii.hex
	avrdude -p m32 -v -c stk500v2 -e -V -U flash:w:segwii.hex


##
## Firmware build
## 
segwii_fw:
	make build -f segwii.mk DEFINES+=-D_DEBUG_ BUILD_TARGET=segwii

segwii_fw_debug:
	make build -f segwii.mk BUILD_TARGET=segwii 
	
##
## Clean
##
clean:
	@echo
	@echo Cleaning up...
	$(REMOVE) *.hex
	$(REMOVE) *.eep
	$(REMOVE) *.obj
	$(REMOVE) *.cof
	$(REMOVE) *.elf
	$(REMOVE) *.map
	$(REMOVE) *.obj
	$(REMOVE) *.a90
	$(REMOVE) *.sym
	$(REMOVE) *.lnk
	$(REMOVE) *.lss
	$(REMOVE) *.o
	$(REMOVE) *.lst
	$(REMOVE) *.s
	$(REMOVE) *.d
	$(REMOVE) .dep/*
	

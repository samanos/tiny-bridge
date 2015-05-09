programmerDev=/dev/spidev0.0
programmerType=linuxspi

optimise=-ffunction-sections -fdata-sections -ffreestanding
cflags=-g -DF_CPU=$(avrFreq) -mmcu=$(avrType) -Wall -Os -Wextra -std=gnu99 -I src
dudeflags=-p $(avrType) -c $(programmerType) -P $(programmerDev) -b 10000

avrType=attiny85
avrFreq=16500000
fuses=-U lfuse:w:0xe1:m -U hfuse:w:0xdd:m

ifdef dudeHost
	# remote avrdude
	sshdude=ssh root@$(dudeHost) /usr/local/bin/avrdude
	dudeFlash=cat $(name).flash.hex $(name).flash.hex | $(sshdude)
	dudeFlashFile=-
	dudeFuse=$(sshdude)
else
	# local avrdude
	dudeFlash=avrdude
	dudeFlashFile=$(name).flash.hex
	dudeFuse=avrdude
endif

name=main
srcs=src/main.c
objs=$(subst .c,.o,$(srcs))
lsts=$(subst .c,.lst,$(srcs))

# definitions that override defines in various libraries
cflags+=-include src/tinytemplateconfig.h

# protocol
cflags+=-I vendor/protocol/src

# vusb lib
vusb=vendor/vusb/usbdrv
cflags+=-I $(vusb)
srcs+= \
	$(vusb)/usbdrv.c \
	$(vusb)/oddebug.c \
	$(vusb)/usbdrvasm.c

# tiny-hw-spi lib
ths=vendor/tiny-hw-spi
cflags+=-I $(ths)
srcs+= \
	$(ths)/tiny_hw_spi.c

# nrf-network
nn=vendor/nrf-network
cflags+= -I $(nn) -I $(nn)/tiny/tinytemplate/firmware/include
srcs+= \
	$(nn)/transmitter.c \
	$(nn)/common.c \
	$(nn)/tiny/platform.c \
	$(nn)/tiny/tinytemplate/firmware/shared/uart_send.c \
	$(nn)/tiny/tinytemplate/firmware/shared/utility.c

.PHONY: clean object elf hex flash readfuse ext-osc-no-divide int-osc-no-divide

clean:
	rm -f $(objs) $(lsts) $(name).elf $(name).eeprom.hex $(name).fuses.hex $(name).lfuse.hex $(name).hfuse.hex $(name).efuse.hex $(name).flash.hex

%.o: %.c
	avr-gcc $(cflags) $(optimise) -Wa,-ahlmns=$(subst .o,.lst,$@) -c -o $@ $<

%.o: %.S
	avr-gcc $(cflags) -x assembler-with-cpp -c $< -o $@

elf: $(objs)
	avr-gcc $(cflags) $(optimise) -o $(name).elf $(objs)
	chmod a-x $(name).elf 2>&1

hex: elf
	avr-objcopy -j .text -j .data -O ihex $(name).elf $(name).flash.hex

flash: hex
	$(dudeFlash) $(dudeflags) -v -U flash:w:$(dudeFlashFile):i

readfuse:
	$(dudeFuse) $(dudeflags) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

writefuse:
	$(dudeFuse) $(dudeflags) -e $(fuses)

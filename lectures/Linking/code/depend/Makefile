NAME = blink

OBJECTS = gpio.o timer.o

CS107E=../../../..
CFLAGS = -I$(CS107E)/libpi/include -g -Wall -Og -std=c99 -ffreestanding 
LDFLAGS = -nostdlib -T memmap -L$(CS107E)/libpi/lib
LDLIBS = -lpi 

all : $(NAME).bin

%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: %.o $(OBJECTS) start.o cstart.o
	arm-none-eabi-ld $(LDFLAGS) $^ $(LDLIBS) -o $@
	arm-none-eabi-objdump -d $@ > $@.list

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@
	arm-none-eabi-gcc -MM $(CFLAGS) -c $< > $*.d

%.o: %.s
	arm-none-eabi-as $< -o $@

%.list: %.o
	arm-none-eabi-objdump -d $< > $@

install: $(NAME).bin
	rpi-install.py $<

test: test.bin
	rpi-install.py $<

clean:
	rm -f *.d *.o *.bin *.elf *.list *~

.PHONY: all clean install test

.PRECIOUS: %.o %.elf

# include dependency info
#  "-" suppresses errors
-include $(NAME).d $(OBJECTS:.o=.d)

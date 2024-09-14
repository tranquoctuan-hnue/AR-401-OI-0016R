CROSS_COMPILE ?=/home/quoctuan/Ambarella_Toolchain_Linaro_2020.09/linaro-2020.09/armv8/aarch64/linaro-aarch64-2020.09-gcc10.2-linux5.4-x86_64/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-gcc
CC = $(CROSS_COMPILE) 
LD = $(CROSS_COMPILE)ld

CFLAGS = $(INCLUDES)

SDK_LIB_DIR = ../inc

SDK_INC_DIR = ../inc

INCLUDES = -I$(SDK_INC_DIR)

#LIBS =	$(SDK_LIB_DIR)/pvn_dfu.h $(SDK_LIB_DIR)/pvn_uart.h $(SDK_LIB_DIR)/pvn_gpio.h

LDFLAG  = -Wl,-gc-sections

SAMPLES = nuvoton_upgrade_2

all: 	$(SAMPLES)

$(SAMPLES): uart.o main.o 
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

# distclean: clean
#  	rm -f $(SAMPLES)

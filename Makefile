CC = m68k-elf-gcc
CFLAGS = -D__KERNEL__ 
#CFLAGS += -D__NO_VERSION__
CFLAGS += -DMODULE
CFLAGS += -DNO_MM
CFLAGS += -Wall
CFLAGS += -O2
CFLAGS += -m68000
CFLAGS += -D__linux__
CFLAGS += -DNO_CACHE
CFLAGS += -DNO_FPU
CFLAGS += -D__ELF__
CFLAGS += -DMAGIC_ROM_PTR
CFLAGS += -DNO_FORGET
CFLAGS += -DUTS_SYSNAME=\"uClinux\"
CFLAGS += -fno-builtin
CFLAGS += -fno-common
CLFAGS += -fomit-frame-pointer
INCDIR = /home/sjm/uClinux-dist/linux-2.4.x/include

all:	
	$(CC) $(CFLAGS) -c dds.c -I$(INCDIR)

clean:
	rm -rf dds.o


# =============================================================================
#
# Make file for BlueBoard examples
#
# Processor: LPC214x
#
# Example:   
#
# Compiler: GCC ARM  3.4.3 (GNUARM + Cygwin) or 4.10
#
# !!!! Do not use  GCC ARM 4.0.1. It makes a few problems  !!!!
#
#  No dependencies support. Call 'make clean' and than 'make' to rebuild all
#
# =============================================================================

# =========  General =============

#FORMAT = ihex
FORMAT = binary
#TST_N  = 1

# =========  Pathes ==============

#ROOTDIR  = D:/CYGWIN
INCLUDES = -I ./ -I ../Source/Platform/Inc 

#TOOLSROOT = D:/GNUARM
TOOLS     =  arm-elf-

PRJPATH   = .

SOURCEPATH = ../Source

DEBUG              = $(PRJPATH)/DEBUG
OBJ                = $(PRJPATH)
PLATFORM_SRC_PATH  = $(SOURCEPATH)/Platform/Src


#===========  Files  ==============

#--  Project target file name (without extension).

IMAGE = app

#--  Linker script

LDSCRIPT = $(PRJPATH)/lpc2148_gcc.ld

#-- List of objects

SRC1=blueboard.c console.c constantpool.c alltests.c debugger.c disassembler.c frame.c heap.c
SRC2=instruction1.c lcd.c main.c operandstack.c util.c xyprintf.c hardware.c gameduino.c
SRC3=Java_java_lang_Class.c Java_java_lang_Object.c 
SRC4=Java_java_io_PrintStream.c
SRC5=rs232.c trace.c nostdlib.c jni.c Java_java_lang_Throwable.c

SOURCES=$(SRC1) $(SRC2) $(SRC3) $(SRC4) $(SRC5)

OBJECTS=$(SOURCES:.c=.o) startup_hardware_init.o tnkernel_startup_gcc.o

# ---------------------------------------------------------------------------
#  Tools, tools options(flags)
# ---------------------------------------------------------------------------

# SHELL   = D:/cygwin/bin/sh
SHELL   = /bin/sh
CC      = $(TOOLS)gcc
AS      = $(TOOLS)as
CPP     = $(TOOLS)g++
LD      = $(TOOLS)ld
OBJCOPY = $(TOOLS)objcopy
OBJDUMP = $(TOOLS)objdump
SIZE    = $(TOOLS)size
STRIP   = $(TOOLS)strip
NM      = $(TOOLS)nm
REMOVE  = rm -f
COPY    = cp


OPTFLAGS = -mcpu=arm7tdmi
DEBUG =

#-- Assembler flags

ADEFS =

#ASMOPTS  = $(OPTFLAGS) $(INCLUDES) -D_GNU_ASSEMBLER_ \
#           -mlittle-endian -nostdinc -x assembler-with-cpp \
#           $(ADEFS)  -Wa,-gstabs


ASMOPTS  =  --traditional-format -march=armv4t -EL

# -Wa,-adhlns=$(<:.S=.lst),-g$(DEBUG)

#-- Compiler  flags

CDEF =

CCOPTS  = $(OPTFLAGS) $(INCLUDES) $(CDEF) -c \
     -mlittle-endian -Wall -gdwarf-2 -std=c99 \
     -march=armv4t -mlittle-endian -fno-builtin

#-- Linker    flags

#LDFLAGS =  -nostartfiles -Wl,-Map=$(OBJ)/$(IMAGE).map,--cref  -lc \
#           $(NEWLIBLPC) -lm -lc -lgcc -T$(LDSCRIPT)
LDFLAGS =  -nostartfiles -Wl,-Map=$(OBJ)/$(IMAGE).map,--cref  \
           -T$(LDSCRIPT) -nostdlib
#           -T$(LDSCRIPT) -nostdlib Her udelades stdlibc
#           $(NEWLIBLPC) -lm -lc -lgcc -T$(LDSCRIPT)


#-- Combine all necessary flags and optional flags.

ALL_CFLAGS =  -I. $(CFLAGS)
ALL_ASFLAGS = -I. $(ASFLAGS)

# ---------------------------------------------------------------------------
#  Build
# ---------------------------------------------------------------------------

all: $(PRJPATH)/$(IMAGE).elf  sizeafter


#--- Compiling/Assembling ---

tnkernel_startup_gcc.o: tnkernel_startup_gcc.s
	$(AS) $(ASMOPTS) tnkernel_startup_gcc.s -o tnkernel_startup_gcc.o
startup_hardware_init.o: startup_hardware_init.s
	$(AS) $(ASMOPTS) startup_hardware_init.s -o startup_hardware_init.o

#---- Compiling Application code -----
#---- Compiling Drivers -----
#---- Compiling EFSL --------
# ---- Compiling LPC usb stack -----
#---- Compiling Platform Specific Code -------
#---- Compiling Utilities sources -----
#---- Compiling Application main ----
.c.o:
	$(CC) $(CCOPTS) $< -o $@

#-- Linking ---

$(PRJPATH)/$(IMAGE).elf: $(OBJECTS)

	$(REMOVE)  $(OBJ)/$(IMAGE).elf
	$(REMOVE)  $(OBJ)/$(IMAGE).bin
	$(REMOVE)  $(OBJ)/$(IMAGE).lst
	$(REMOVE)  $(OBJ)/$(IMAGE).map

	$(CC) $(ALL_CFLAGS) $(OBJECTS) --output $(OBJ)/$(notdir $@) $(LDFLAGS)

	$(OBJCOPY) -O$(FORMAT)    $(OBJ)/$(IMAGE).elf  $(OBJ)/$(IMAGE).bin
#	$(NM) -n $(OBJ)/$(IMAGE).elf

#--- Display size of file.

#HEXSIZE = $(SIZE) --target=$(FORMAT) $(OBJ)/$(IMAGE).hex
ELFSIZE = $(SIZE) -A $(OBJ)/$(IMAGE).elf

sizeafter:
	@if [ -f $(OBJ)/$(IMAGE).elf ]; then echo; $(ELFSIZE); echo; fi


# ===========================================================================

.PHONY:   clean

clean:

	$(REMOVE) $(OBJECTS)

# ===========================================================================
# ===========================================================================
# ===========================================================================



#	$(STRIP) $(OBJ)/$(IMAGE).elf

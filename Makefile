AMIGA_GCC_ROOT=./amiga-gcc
AMIGA_GCC_BIN=$(AMIGA_GCC_ROOT)/bin
CC=$(AMIGA_GCC_BIN)/m68k-amigaos-gcc
LDFLAGS=-lamiga -lauto
ODIR=build-gcc
VASM=$(AMIGA_GCC_BIN)/vasmm68k_mot
NAME=pixelcube
EXE=/Volumes/share/amiga-c/$(NAME)
ASM=$(NAME).s
_OBJ = pixelcube.o start.o line.o ptplayer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
CFLAGS = -g0 -O3 -m68030 -Ilibs/SDI/includes -Wno-int-to-pointer-cast
LDLIBS =

# Prepare variables for target 'clean'
ifeq ($(OS),Windows_NT)
	RM:=del
	PATHSEP:=\\
	CONFIG:=${CONFIG}_win
else
	RM:=rm -f 
	PATHSEP:=/
endif

all: $(EXE)

$(EXE) : $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) -o $(EXE)

$(ODIR)/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(CC) $(CFLAGS) -S -o $(ODIR)/asm/$*.s $<

$(ODIR)/%.o : %.asm
	$(VASM) -Fhunk -m68030 -Iamiga-gcc/m68k-amigaos/ndk-include -o $@ $<

clean:
	-$(RM) $(ODIR)$(PATHSEP)*.o
	-$(RM) $(subst /,$(PATHSEP),$(EXE))
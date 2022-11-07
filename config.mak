CROSS = m68k-atari-mint-

CC = $(CROSS)gcc
AS = $(CC)

OPT = -O2 -fomit-frame-pointer -mshort
WARN =
# disable for now until usage of int for AES/VDI functions has been fixed
WARN += -Wno-incompatible-pointer-types
DEFS =
INCLUDES = -I.
CFLAGS = $(OPT) $(WARN) $(DEFS) $(INCLUDES)
LDFLAGS =
LIBS = -lgem

.s.o:
	$(CC) $(CFLAGS) -c -o $@ $<


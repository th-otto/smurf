CROSS = m68k-atari-mint-

CC = $(CROSS)gcc
AS = $(CC)
AR = $(CROSS)ar

OPT = -O2 -fomit-frame-pointer
WARN = -Werror
DEFS =
INCLUDES = -I.
CFLAGS = $(OPT) $(WARN) $(DEFS) $(INCLUDES)
LDFLAGS =
LIBS = -lgem

DISTDIR = $(top_srcdir)/dist

.s.o:
	$(CC) $(CFLAGS) -c -o $@ $<


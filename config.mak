CROSS = m68k-atari-mint-

CC = $(CROSS)gcc
AS = $(CC)
AR = $(CROSS)ar

OPT = -O2 -fomit-frame-pointer -Wstrict-prototypes -Wmissing-prototypes
WARN = -Werror
DEFS =
INCLUDES = -I. -I$(top_srcdir)/include
CFLAGS = $(OPT) $(WARN) $(DEFS) $(INCLUDES)
ASFLAGS = -Wa,--defsym,PURE_C=0
LDFLAGS = -s
LIBS = -lgem

DISTDIR = $(top_srcdir)/dist

.s.o:
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<


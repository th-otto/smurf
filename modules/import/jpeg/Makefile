top_srcdir=../../..
include $(top_srcdir)/config.mak

INCLUDES += -I$(top_srcdir)/src/lib -I$(top_srcdir)/src -I$(top_srcdir)/modules/dither
LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/jpeg.sim \
	$(DISTDIR)/modules/import/jpegdsp.sim \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/import/jpeg.sim: ../impstart.o jpegimp2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -ljpeg $(LIBS)

$(DISTDIR)/modules/import/jpegdsp.sim: ../impstart.o jpegdsp.o jpegdsps.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

jpegdsps.o: jpegdsps.s
	$(CC) $(CFLAGS) $(ASFLAGS) -m68020 -c -o $@ $<

clean::
	$(RM) *.o $(MODULES)

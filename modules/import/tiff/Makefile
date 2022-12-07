top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/tiff.sim \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/import/tiff.sim: ../impstart.o tiffimp.o tiffmod.o setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

setlines.o: $(top_srcdir)/src/setlines.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

clean::
	$(RM) *.o ../impstart.o $(MODULES)

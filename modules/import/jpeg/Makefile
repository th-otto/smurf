top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/jpeg.sim \
	$(DISTDIR)/modules/import/jpegdsp.sim \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/import/jpeg.sim: ../impstart.o jpegimp2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -ljpeg $(LIBS)

$(DISTDIR)/modules/import/jpegdsp.sim: ../impstart.o jpegdsp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(MODULES)

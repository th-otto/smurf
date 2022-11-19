top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/dither/nc.sdm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/dither/nc.sdm: ../ditstart.o nc.o line_dit.o nc1.o ../../../src/getpix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../ditstart.o ../../../src/getpix.o $(MODULES)

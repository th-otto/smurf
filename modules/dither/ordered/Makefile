top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/dither/ordered.sdm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/dither/ordered.sdm: ../ditstart.o ordered.o ../../../src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../ditstart.o ../../../src/getlines.o $(MODULES)

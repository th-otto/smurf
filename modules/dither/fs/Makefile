top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/dither/floydst.sdm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/dither/floydst.sdm: ../ditstart.o floydst.o fs_68000.o ../../../src/getpix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../ditstart.o ../../../src/getpix.o $(MODULES)

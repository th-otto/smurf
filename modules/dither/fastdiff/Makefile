top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/dither/fdfast.sdm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/dither/fdfast.sdm: ../ditstart.o fdfast.o ../../../src/getpix.o fdd030.o line_dit.o fdf_scol.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../ditstart.o ../../../src/getpix.o $(MODULES)

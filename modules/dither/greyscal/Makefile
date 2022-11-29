top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/dither/greyscal.sdm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/dither/greyscal.sdm: ../ditstart.o greyscal.o setpix16.o $(top_srcdir)/src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../ditstart.o $(top_srcdir)/src/getlines.o $(MODULES)

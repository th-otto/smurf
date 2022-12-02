top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/ppm.sxm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/export/ppm.sxm: ../expstart.o ppm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../expstart.o $(MODULES)

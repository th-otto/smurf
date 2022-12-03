top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/trp.sxm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/export/trp.sxm: ../expstart.o trp.o $(top_srcdir)/src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../expstart.o $(top_srcdir)/src/getlines.o $(MODULES)

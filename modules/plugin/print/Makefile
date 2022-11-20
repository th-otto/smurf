top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/plugin/print.plg \
	$(DISTDIR)/modules/plugin/printwd.plx \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/plugin/print.plg: ../plgstart.o print.o gdos.o bindings.o ../../../src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/plugin/printwd.plx: ../plgstart.o printw.o gdosw.o bindings.o ../../../src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../plgstart.o ../../../src/getlines.o $(MODULES)

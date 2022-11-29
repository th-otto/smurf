top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/plugin/print.plg \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/plugin/print.plg: ../plgstart.o print.o gdos.o bindings.o $(top_srcdir)/src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../plgstart.o $(top_srcdir)/src/getlines.o $(MODULES)

top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/plugin/multicon.plg \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/plugin/multicon.plg: ../plgstart.o multicon.o convert.o tools.o wildcard.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../plgstart.o $(MODULES)

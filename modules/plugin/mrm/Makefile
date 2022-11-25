top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/plugin/mrm.plg \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/plugin/mrm.plg: ../plgstart.o mrm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../plgstart.o $(MODULES)

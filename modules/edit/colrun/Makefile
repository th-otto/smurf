top_srcdir=../../..
include $(top_srcdir)/config.mak

INCLUDES += -I$(top_srcdir)/src/lib -I$(top_srcdir)/src -I$(top_srcdir)/modules/dither
LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/colrun.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/colrun.sem: ../edstart.o colrun.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(MODULES)

top_srcdir=../..
include $(top_srcdir)/config.mak

INCLUDES += -I$(top_srcdir)/src/lib -I$(top_srcdir)/src -I$(top_srcdir)/modules/dither
LIBS += -lgem -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/icn.sxm \
	$(DISTDIR)/modules/export/esm.sxm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/export/icn.sxm: expstart.o icn.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/esm.sxm: expstart.o esm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(MODULES)

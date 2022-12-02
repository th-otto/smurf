top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/falc16.sxm \
	$(DISTDIR)/modules/export/falc8bit.sxm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/export/falc16.sxm: ../expstart.o falc16.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/falc8bit.sxm: ../expstart.o falc8bit.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../expstart.o $(MODULES)

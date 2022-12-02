top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/pcx.sxm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/export/pcx.sxm: ../expstart.o pcx.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../expstart.o $(MODULES)

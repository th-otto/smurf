top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/pac.sxm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/export/pac.sxm: ../expstart.o pac.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../expstart.o $(MODULES)

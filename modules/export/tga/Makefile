top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/tga.sxm \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/export/tga.sxm: ../expstart.o tga.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../expstart.o $(MODULES)

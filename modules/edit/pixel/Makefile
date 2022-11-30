top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/pixel.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/pixel.sem: ../edstart.o pixel.o pixelass.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../edstart.o $(MODULES)

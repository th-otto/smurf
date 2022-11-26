top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/magicpic.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/magicpic.sem: ../edstart.o magicp_4.o rechne04.o cpypixel.o getpixel.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../edstart.o $(MODULES)

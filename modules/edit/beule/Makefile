top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/beule.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/beule.sem: ../edstart.o bang5.o bang5f.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

bang5.o: bang5.c bangcalc.c

bang5f.o: bang5f.c bangcalc.c
	$(CC) $(CFLAGS) -m68020-60 -c -o $@ $<

clean::
	$(RM) *.o ../edstart.o $(MODULES)

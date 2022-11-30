top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/sphere.sem \
	$(DISTDIR)/modules/edit/spherfpu.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/sphere.sem: ../edstart.o sphere.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/spherfpu.sem: ../edstart.o sphrfpu.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

sphrfpu.o: sphrfpu.c sphere.c
	$(CC) $(CFLAGS) -m68020-60 -c -o $@ $<

clean::
	$(RM) *.o ../edstart.o $(MODULES)

top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/bumpmap.sem \
	$(DISTDIR)/modules/edit/bumber.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/bumpmap.sem: ../edstart.o bumpmap.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/bumber.sem: ../edstart.o bumber.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../edstart.o $(MODULES)

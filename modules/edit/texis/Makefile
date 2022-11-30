top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/texis.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/texis.sem: ../edstart.o texis.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../edstart.o $(MODULES)

top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/cchange.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/cchange.sem: ../edstart.o cchange.o $(top_srcdir)/src/getlines.o $(top_srcdir)/src/setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../edstart.o $(top_srcdir)/src/getlines.o $(top_srcdir)/src/setlines.o $(MODULES)

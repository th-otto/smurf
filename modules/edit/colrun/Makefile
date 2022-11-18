top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/colrun.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/colrun.sem: ../edstart.o colrun.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(MODULES)

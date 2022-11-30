top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/scheren.sem \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/edit/scheren.sem: ../edstart.o scheren.o scherass.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../edstart.o $(MODULES)

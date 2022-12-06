top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/mim.sim \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/import/mim.sim: ../impstart.o mim_5.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o ../impstart.o $(MODULES)

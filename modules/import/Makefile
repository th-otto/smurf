top_srcdir=../..
include $(top_srcdir)/config.mak

INCLUDES += -I$(top_srcdir)/src/lib -I$(top_srcdir)/src -I$(top_srcdir)/modules/dither
LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/godpaint.sim \
	$(DISTDIR)/modules/import/pixar.sim \
	$(DISTDIR)/modules/import/qdv.sim \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/import/godpaint.sim: impstart.o godpaint.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pixar.sim: impstart.o pixar.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/qdv.sim: impstart.o qdv.o setlintc.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

setlines.o: $(top_srcdir)/src/setlines.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

setlintc.o: $(top_srcdir)/src/setlintc.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

clean::
	$(RM) *.o $(MODULES)

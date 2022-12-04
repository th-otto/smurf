top_srcdir=../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/1stpb.sim \
	$(DISTDIR)/modules/import/adexv1.sim \
	$(DISTDIR)/modules/import/alphabmp.sim \
	$(DISTDIR)/modules/import/bmp.sim \
	$(DISTDIR)/modules/import/bsgv1.sim \
	$(DISTDIR)/modules/import/esm.sim \
	$(DISTDIR)/modules/import/godpaint.sim \
	$(DISTDIR)/modules/import/pixar.sim \
	$(DISTDIR)/modules/import/qdv.sim \
	$(DISTDIR)/modules/import/qfax.sim \
	$(DISTDIR)/modules/import/softimag.sim \
	$(DISTDIR)/modules/import/spectrum.sim \
	$(empty)

SUBDIRS = jpeg

all:: impstart.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { exit 1; }; done

$(DISTDIR)/modules/import/1stpb.sim: impstart.o 1stpb.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/adexv1.sim: impstart.o adexv1.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/alphabmp.sim: impstart.o alphabmp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/bmp.sim: impstart.o bmp.o setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/bsgv1.sim: impstart.o bsgv1.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/esm.sim: impstart.o esm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/godpaint.sim: impstart.o godpaint.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/jpeg.sim: impstart.o jpeg/jpegimp2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -ljpeg $(LIBS)

$(DISTDIR)/modules/import/jpegdsp.sim: impstart.o jpeg/jpegdsp.o jpeg/jpegdsps.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pixar.sim: impstart.o pixar.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/qdv.sim: impstart.o qdv.o setlintc.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/qfax.sim: impstart.o qfax.o timer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/softimag.sim: impstart.o softimag.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/spectrum.sim: impstart.o spectrum.o getpix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)


setlines.o: $(top_srcdir)/src/setlines.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

setlintc.o: $(top_srcdir)/src/setlintc.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

getpix.o: $(top_srcdir)/src/getpix.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

timer.o: $(top_srcdir)/src/lib/timer.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean::
	$(RM) *.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

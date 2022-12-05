top_srcdir=../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/1stpb.sim \
	$(DISTDIR)/modules/import/adexv.sim \
	$(DISTDIR)/modules/import/alphabmp.sim \
	$(DISTDIR)/modules/import/bmp.sim \
	$(DISTDIR)/modules/import/bsgv1.sim \
	$(DISTDIR)/modules/import/bw.sim \
	$(DISTDIR)/modules/import/cas.sim \
	$(DISTDIR)/modules/import/ceg.sim \
	$(DISTDIR)/modules/import/clipboar.sim \
	$(DISTDIR)/modules/import/cmp.sim \
	$(DISTDIR)/modules/import/cut.sim \
	$(DISTDIR)/modules/import/dip.sim \
	$(DISTDIR)/modules/import/doodle.sim \
	$(DISTDIR)/modules/import/dta.sim \
	$(DISTDIR)/modules/import/egarx.sim \
	$(DISTDIR)/modules/import/eps.sim \
	$(DISTDIR)/modules/import/esm.sim \
	$(DISTDIR)/modules/import/fal.sim \
	$(DISTDIR)/modules/import/fckpaint.sim \
	$(DISTDIR)/modules/import/gem_img.sim \
	$(DISTDIR)/modules/import/gfx.sim \
	$(DISTDIR)/modules/import/godpaint.sim \
	$(DISTDIR)/modules/import/goes.sim \
	$(DISTDIR)/modules/import/gra.sim \
	$(DISTDIR)/modules/import/hilight.sim \
	$(DISTDIR)/modules/import/hsi_raw.sim \
	$(DISTDIR)/modules/import/ibas_img.sim \
	$(DISTDIR)/modules/import/ibm_pic.sim \
	$(DISTDIR)/modules/import/icn.sim \
	$(DISTDIR)/modules/import/iff.sim \
	$(DISTDIR)/modules/import/lan.sim \
	$(DISTDIR)/modules/import/pixar.sim \
	$(DISTDIR)/modules/import/qdv.sim \
	$(DISTDIR)/modules/import/qfax.sim \
	$(DISTDIR)/modules/import/softimag.sim \
	$(DISTDIR)/modules/import/spectrum.sim \
	$(empty)

SUBDIRS = jpeg gif_ii

all:: impstart.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { exit 1; }; done

$(DISTDIR)/modules/import/1stpb.sim: impstart.o 1stpb.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/adexv.sim: impstart.o adexv1.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/alphabmp.sim: impstart.o alphabmp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/bmp.sim: impstart.o bmp.o setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/bsgv1.sim: impstart.o bsgv1.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/bw.sim: impstart.o bw.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/cas.sim: impstart.o cas.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/ceg.sim: impstart.o ceg.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/clipboar.sim: impstart.o clipboar.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/cmp.sim: impstart.o cmp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/cut.sim: impstart.o cut.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/dip.sim: impstart.o dip.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/doodle.sim: impstart.o doodle.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/dta.sim: impstart.o dta.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/egarx.sim: impstart.o egarx.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/eps.sim: impstart.o eps.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/esm.sim: impstart.o esm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/fal.sim: impstart.o fal.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/fckpaint.sim: impstart.o fckpaint.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/gem_img.sim: impstart.o gem_img.o getinter.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/gfx.sim: impstart.o gfx.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/godpaint.sim: impstart.o godpaint.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/goes.sim: impstart.o goes.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/gra.sim: impstart.o gra.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/hilight.sim: impstart.o hilight.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/hsi_raw.sim: impstart.o hsi_raw.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/ibas_img.sim: impstart.o ibas_img.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/ibm_pic.sim: impstart.o ibm_pic.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/icn.sim: impstart.o icn.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/iff.sim: impstart.o iff.o getinter.o getint08.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/lan.sim: impstart.o lan.o
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

getinter.o: ../getinter.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

getint08.o: ../getint08.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

clean::
	$(RM) *.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

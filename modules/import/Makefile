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
	$(DISTDIR)/modules/import/iim.sim \
	$(DISTDIR)/modules/import/indy.sim \
	$(DISTDIR)/modules/import/indy16.sim \
	$(DISTDIR)/modules/import/lan.sim \
	$(DISTDIR)/modules/import/mac.sim \
	$(DISTDIR)/modules/import/msp.sim \
	$(DISTDIR)/modules/import/mtv_mod.sim \
	$(DISTDIR)/modules/import/neoch.sim \
	$(DISTDIR)/modules/import/neonmap.sim \
	$(DISTDIR)/modules/import/pcd.sim \
	$(DISTDIR)/modules/import/pcp.sim \
	$(DISTDIR)/modules/import/pcx.sim \
	$(DISTDIR)/modules/import/pic_pcpa.sim \
	$(DISTDIR)/modules/import/pict.sim \
	$(DISTDIR)/modules/import/pix.sim \
	$(DISTDIR)/modules/import/pixar.sim \
	$(DISTDIR)/modules/import/pofo.sim \
	$(DISTDIR)/modules/import/ppm.sim \
	$(DISTDIR)/modules/import/psd.sim \
	$(DISTDIR)/modules/import/qdv.sim \
	$(DISTDIR)/modules/import/qfax.sim \
	$(DISTDIR)/modules/import/qrt.sim \
	$(DISTDIR)/modules/import/r8.sim \
	$(DISTDIR)/modules/import/ras.sim \
	$(DISTDIR)/modules/import/raw.sim \
	$(DISTDIR)/modules/import/rawmod.sim \
	$(DISTDIR)/modules/import/scodl.sim \
	$(DISTDIR)/modules/import/sef.sim \
	$(DISTDIR)/modules/import/sgi.sim \
	$(DISTDIR)/modules/import/smurf_mt.six \
	$(DISTDIR)/modules/import/softimag.sim \
	$(DISTDIR)/modules/import/spectrum.sim \
	$(DISTDIR)/modules/import/stad.sim \
	$(DISTDIR)/modules/import/tga.sim \
	$(DISTDIR)/modules/import/tny.sim \
	$(DISTDIR)/modules/import/tpi_true.sim \
	$(DISTDIR)/modules/import/tpi_turb.sim \
	$(DISTDIR)/modules/import/trp_all.sim \
	$(DISTDIR)/modules/import/truepnt.sim \
	$(DISTDIR)/modules/import/vidtex.sim \
	$(DISTDIR)/modules/import/vivi_img.sim \
	$(DISTDIR)/modules/import/xbm.sim \
	$(empty)

#MODULES +=	$(DISTDIR)/modules/import/neonshp.sim

SUBDIRS = jpeg gif_ii mim tiff

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

$(DISTDIR)/modules/import/iim.sim: impstart.o iim.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/indy.sim: impstart.o indy.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/indy16.sim: impstart.o indy16.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/lan.sim: impstart.o lan.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/mac.sim: impstart.o mac.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/msp.sim: impstart.o msp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/mtv_mod.sim: impstart.o mtv_mod.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/neoch.sim: impstart.o neoch.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/neonmap.sim: impstart.o neonmap.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/neonshp.sim: impstart.o neonshp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pcd.sim: impstart.o pcd.o pcddec00.o pcddec20.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pcp.sim: impstart.o pcp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pcx.sim: impstart.o pcx.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pic_pcpa.sim: impstart.o pic_pcpa.o setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pict.sim: impstart.o pict.o setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pix.sim: impstart.o pix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pixar.sim: impstart.o pixar.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/pofo.sim: impstart.o pofo.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/ppm.sim: impstart.o ppm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/psd.sim: impstart.o psd.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/qdv.sim: impstart.o qdv.o setlintc.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/qfax.sim: impstart.o qfax.o timer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/qrt.sim: impstart.o qrt.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/r8.sim: impstart.o r8.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/ras.sim: impstart.o ras.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/raw.sim: impstart.o raw.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/rawmod.sim: impstart.o rawmod.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/scodl.sim: impstart.o scodl.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/sef.sim: impstart.o sef.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/sgi.sim: impstart.o sgi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/smurf_mt.six: impstart.o smurf_mt.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/softimag.sim: impstart.o softimag.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/spectrum.sim: impstart.o spectrum.o getpix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/stad.sim: impstart.o stad.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/tga.sim: impstart.o tga.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/tny.sim: impstart.o tny.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/tpi_true.sim: impstart.o tpi_true.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/tpi_turb.sim: impstart.o tpi_turb.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/trp_all.sim: impstart.o trp_all.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/truepnt.sim: impstart.o truepnt.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/vidtex.sim: impstart.o vidtex.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/vivi_img.sim: impstart.o vivi_img.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/import/xbm.sim: impstart.o xbm.o
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

pcddec00.o: pcddec00.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

pcddec20.o: pcddec20.s
	$(CC) $(CFLAGS) $(ASFLAGS) -m68020 -c -o $@ $<

clean::
	$(RM) *.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

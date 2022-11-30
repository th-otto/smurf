top_srcdir=../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/border.sem \
	$(DISTDIR)/modules/edit/clip.sem \
	$(DISTDIR)/modules/edit/clipnpic.sem \
	$(DISTDIR)/modules/edit/colsat.sem \
	$(DISTDIR)/modules/edit/displace.sem \
	$(DISTDIR)/modules/edit/dropshdw.sem \
	$(DISTDIR)/modules/edit/edgeokil.sem \
	$(DISTDIR)/modules/edit/equalize.sem \
	$(DISTDIR)/modules/edit/erode.sem \
	$(DISTDIR)/modules/edit/expander.sem \
	$(DISTDIR)/modules/edit/extrapol.sem \
	$(DISTDIR)/modules/edit/gamma.sem \
	$(DISTDIR)/modules/edit/grau.sem \
	$(DISTDIR)/modules/edit/highshad.sem \
	$(DISTDIR)/modules/edit/interpo.sem \
	$(DISTDIR)/modules/edit/invert.sem \
	$(DISTDIR)/modules/edit/konform.sem \
	$(DISTDIR)/modules/edit/litlgrad.sem \
	$(DISTDIR)/modules/edit/minmax.sem \
	$(DISTDIR)/modules/edit/noise.sem \
	$(DISTDIR)/modules/edit/ntsc.sem \
	$(DISTDIR)/modules/edit/paint.sem \
	$(DISTDIR)/modules/edit/paletout.sem \
	$(DISTDIR)/modules/edit/quicksmt.sem \
	$(DISTDIR)/modules/edit/rahmen.sem \
	$(DISTDIR)/modules/edit/scale.sem \
	$(DISTDIR)/modules/edit/speedom.sem \
	$(DISTDIR)/modules/edit/sw.sem \
	$(empty)

# MODULES += $(DISTDIR)/modules/edit/pepsip.sem # does not do anything
# MODULES += $(DISTDIR)/modules/edit/picfade.sem # does not do anything

SUBDIRS = beule bumpmap cchange chanel5 chanelmx colrun freefilt hsv magic mirror pixel rotate

all:: edstart.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { exit 1; }; done

$(DISTDIR)/modules/edit/border.sem: edstart.o border.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/clip.sem: edstart.o clip.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/clipnpic.sem: edstart.o clipnpic.o $(top_srcdir)/src/getlines.o $(top_srcdir)/src/getpix.o $(top_srcdir)/src/rearr.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/colsat.sem: edstart.o colsat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/displace.sem: edstart.o displace.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/dropshdw.sem: edstart.o dropshdw.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/edgeokil.sem: edstart.o edgeokil.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/equalize.sem: edstart.o equalize.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/erode.sem: edstart.o erode.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/expander.sem: edstart.o expander.o $(top_srcdir)/src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/extrapol.sem: edstart.o extrapol.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/gamma.sem: edstart.o gamma.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/grau.sem: edstart.o grau.o $(top_srcdir)/src/getlines.o $(top_srcdir)/src/setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/highshad.sem: edstart.o highshad.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/interpo.sem: edstart.o interpo.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/invert.sem: edstart.o invert.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/konform.sem: edstart.o konform.o $(top_srcdir)/src/getlines.o $(top_srcdir)/src/setlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/litlgrad.sem: edstart.o litlgrad.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/minmax.sem: edstart.o minmax.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/noise.sem: edstart.o noise.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/ntsc.sem: edstart.o ntsc.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/paint.sem: edstart.o paint.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/paletout.sem: edstart.o paletout.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/pepsip.sem: edstart.o pepsip.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/picfade.sem: edstart.o picfade.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/rahmen.sem: edstart.o rahmen.o $(top_srcdir)/src/getlines.o $(top_srcdir)/src/setlines.o $(top_srcdir)/src/rearr.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/quicksmt.sem: edstart.o quicksmt.o smth_mat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/scale.sem: edstart.o scale.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/speedom.sem: edstart.o speedom.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/sw.sem: edstart.o sw.o $(top_srcdir)/src/getlines.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(top_srcdir)/src/getlines.o $(top_srcdir)/src/setlines.o $(top_srcdir)/src/getpix.o $(top_srcdir)/src/rearr.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

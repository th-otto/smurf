top_srcdir=../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/export/clipbrd.sxm \
	$(DISTDIR)/modules/export/cmp.sxm \
	dummy.sxm \
	$(DISTDIR)/modules/export/esm.sxm \
	$(DISTDIR)/modules/export/godpaint.sxm \
	$(DISTDIR)/modules/export/icn.sxm \
	$(DISTDIR)/modules/export/iim.sxm \
	$(DISTDIR)/modules/export/pixar.sxm \
	$(empty)

SUBDIRS = bmp falc_raw gem_img gif pac

all:: expstart.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { exit 1; }; done

$(DISTDIR)/modules/export/clipbrd.sxm: expstart.o clipbrd.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/cmp.sxm: expstart.o cmp.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

dummy.sxm: expstart.o dummy.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/esm.sxm: expstart.o esm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/godpaint.sxm: expstart.o godpaint.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/icn.sxm: expstart.o icn.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/iim.sxm: expstart.o iim.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/export/pixar.sxm: expstart.o pixar.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

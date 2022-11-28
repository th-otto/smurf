top_srcdir=../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/border.sem \
	$(DISTDIR)/modules/edit/clip.sem \
	$(DISTDIR)/modules/edit/displace.sem \
	$(DISTDIR)/modules/edit/dropshdw.sem \
	$(DISTDIR)/modules/edit/highshad.sem \
	$(DISTDIR)/modules/edit/quicksmt.sem \
	$(DISTDIR)/modules/edit/speedom.sem \
	$(empty)

SUBDIRS = beule bumpmap colrun freefilt hsv magic

all:: edstart.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { exit 1; }; done

$(DISTDIR)/modules/edit/border.sem: edstart.o border.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/clip.sem: edstart.o clip.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/displace.sem: edstart.o displace.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/dropshdw.sem: edstart.o dropshdw.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/highshad.sem: edstart.o highshad.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/quicksmt.sem: edstart.o quicksmt.o smth_mat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(DISTDIR)/modules/edit/speedom.sem: edstart.o speedom.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

top_srcdir=../..
include $(top_srcdir)/config.mak

INCLUDES += -I$(top_srcdir)/src/lib -I$(top_srcdir)/src -I$(top_srcdir)/modules/dither
LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/edit/clip.sem \
	$(empty)

SUBDIRS = colrun

all:: edstart.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { false; }; done

$(DISTDIR)/modules/edit/clip.sem: edstart.o clip.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean::
	$(RM) *.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

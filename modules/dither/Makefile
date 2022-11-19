top_srcdir=../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(empty)

SUBDIRS = fastdiff fs greyscal nc ordered

all:: ditstart.o ../../src/getpix.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { exit 1; }; done

clean::
	$(RM) *.o ../../src/getpix.o $(MODULES)
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { false; }; done

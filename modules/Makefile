SUBDIRS = dither edit export import plugin

all clean::
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { exit 1; }; done

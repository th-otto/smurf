SUBDIRS = src modules

all clean::
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || { false; }; done

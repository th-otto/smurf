SUBDIRS = src

all clean::
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

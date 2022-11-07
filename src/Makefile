top_srcdir=..
include $(top_srcdir)/config.mak

INCLUDES += -Irsc -Ilib -I$(top_srcdir)/modules/dither

OBJS = \
	modsmurf.o \
	fload.o \
	file_sel.o \
	do_pcd.o \
	config.o \
	keyboard.o \
	shortc2.o \
	f_scan.o \
	exp_form.o \
	export.o \
	picman.o \
	thumbnai.o \
	prefdial.o \
	window.o \
	picevent.o \
	busybox.o \
	edit.o \
	options.o \
	dispopts.o \
	undo.o \
	block.o \
	rtprevs.o \
	smurfobs.o \
	\
	module.o \
	modconf.o \
	plugin/plugin.o \
	\
	xrsrc.o \
	menu.o \
	objct.o \
	resource.o \
	lib/radiochk.o \
	lib/sym_gem.o \
	messageh.o \
	colorch.o \
	\
	dither.o \
	dit_subd.o \
	d2s.o \
	transfm.o \
	\
	medicut/histogrm.o \
	medicut/medicut.o \
	seekcol.o \
	\
	convert.o \
	bubblegm.o \
	lib/slb_bind.o \
	\
	pixelins.o \
	getpix.o \
	setpix.o \
	dit_read.o \
	ditreadp.o \
	set_pixl.o \
	setlines.o \
	getlines.o \
	rearr.o \
	tools.o \
	bindings.o \
	comm.o \
	destruct.o \
	roto/roto1.o \
	olga.o \
	debug.o \
	\
	$(empty)

all: smurf.prg


smurf.prg: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean::
	$(RM) $(OBJS) *.prg *.pdb

top_srcdir=..
include $(top_srcdir)/config.mak

INCLUDES += -Irsc -Ilib -I$(top_srcdir)/modules/dither
LIBS += -lm

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

#	dit_read.o \
#	ditreadp.o \

LIB_OBJS = \
	lib/screens.o \
	lib/ice2.o \
	lib/super.o \
	lib/timer.o \
	lib/dspmod.o \
	$(empty)

all: $(DISTDIR)/smurf.prg


smurf.prg: lib/demolib.a $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) lib/demolib.a $(LIBS)

lib/demolib.a: $(LIB_OBJS)
	@$(RM) $@
	$(AR) rcs $@ $(LIB_OBJS)

clean::
	$(RM) $(OBJS) $(LIB_OBJS) lib/demolib.a $(DISTDIR)/smurf.prg *.pdb

top_srcdir=../../..
include $(top_srcdir)/config.mak

LIBS += -lm
LDFLAGS += -nostartfiles

MODULES = \
	$(DISTDIR)/modules/import/gif.sim \
	$(empty)

all:: $(MODULES)

$(DISTDIR)/modules/import/gif.sim: ../impstart.o gif_2.o gif_2lzw.o giflzw00.o giflzw20.o setpix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

giflzw00.o: giflzw00.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

giflzw20.o: giflzw20.s
	$(CC) $(CFLAGS) $(ASFLAGS) -m68020 -c -o $@ $<

setpix.o: $(top_srcdir)/src/setpix.s
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

clean::
	$(RM) *.o ../impstart.o $(MODULES)

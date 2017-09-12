CC      := gcc
CFLAGS  := -Wall -Wextra
DEFINES := -DMAX_CIRCBUFF_SIZE=64 -DDEBUG
APPNAME := circbuff

$(APPNAME): main.o circbuff.o
	$(CC) -o $@ $^

main.o: main.c circbuff.h common.h Makefile
	$(CC) -c $(CFLAGS) $(DEFINES) -o $@ main.c

circbuff.o: circbuff.c circbuff.h common.h Makefile
	$(CC) -c $(CFLAGS) $(DEFINES) -o $@ circbuff.c

.PHONY: clean distclean

clean:
	rm -rf $(APPNAME) *.o

distclean: clean
	rm -rf *~ log*

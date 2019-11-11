PROG		= gpio
CFLAGS		= -O -Wall -Werror
LIB		=
OBJ		= main.o gpio.o alt.o

ifdef DESTDIR
bindir		= $(DESTDIR)/usr/bin
else
bindir		= /usr/local/bin
endif

all:		$(PROG)

gpio:		$(OBJ)
		$(CC) $(LDFLAGS) $(OBJ) $(LIB) -o $@

clean:
		rm -f $(PROG) *.o

install:	gpio
		mkdir -p $(bindir)
		install -m 4755 gpio $(bindir)/gpio

###
alt.o: alt.c gpio.h
gpio.o: gpio.c gpio.h
main.o: main.c gpio.h

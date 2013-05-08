TARG = rmtmp
OFILES = rmtmp.o
BIN = /usr/local/bin
MAN = /usr/local/man/man1
CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
		-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations	\
		-Wnested-externs -Winline -Wno-long-long  -Wunused-variable	\
		-Wstrict-prototypes -Werror -ansi -D_XOPEN_SOURCE=700		\
		-D_BSD_SOURCE -ansi
LDFLAGS = -static
$TARG::$OFILES

all::$TARG

install:V:$TARG
	install $TARG $BIN
	install $TARG.1 $MAN

installall:

clean:V:
	rm -f *.o

nuke:V:clean
	rm -f $TARG

%.o: %.c
	$CC $CFLAGS -c $stem.c

&: %.o
	$CC $LDFLAGS -o $stem $stem.o

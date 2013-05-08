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

installall:V:install

uninstall:V:
        rm -f $BIN/$TARG
        rm -f $MAN/$TARG.1

uninstallall:V:uninstall

clean:V:
	rm -f *.o $TARG *core*

nuke:V:clean

%.o: %.c
	$CC $CFLAGS -c $stem.c

&: %.o
	$CC $LDFLAGS -o $stem $stem.o

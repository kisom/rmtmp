TARG = rmtmp
OFILES = rmtmp.o
BIN = /usr/local/bin
MAN = /usr/local/man/man1
CC = clang
LD = clang
CFLAGS = -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align        \
		-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations   \
		-Wnested-externs -Winline -Wno-long-long  -Wunused-variable   \
		-Wstrict-prototypes -Werror -ansi -D_XOPEN_SOURCE=700         \
		-D_BSD_SOURCE -ansi -static
LDFLAGS = -static
$TARG::$OFILES

all::$TARG

install:V:$TARG
        install -d $BIN
	install $TARG $BIN
        install -d $MAN
	install $TARG.1 $MAN

installall:V:install

uninstall:V:
        rm -f $BIN/$TARG
        rm -f $MAN/$TARG.1

uninstallall:V:uninstall

clean:V:
	rm -f *.o $TARG *core*
        rm -fr testdata

nuke:V:clean

testdata:V:
        if [ ! -d testdata ]; then
                mkdir testdata
        fi
        for i in $(seq 1 100000)
        do
                touch testdata/test$i
        done

%.o: %.c
	$CC $CFLAGS -c $stem.c

&: %.o
	$CC $LDFLAGS -o $stem $stem.o

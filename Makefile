TARGET = rmtmp
OBJS = rmtmp.o
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


all: $(TARGET)

install: $(TARGET)
	install -d $(BIN)
	install $(TARGET) $(BIN)
	install -d $(MAN)
	install $(TARGET).1 $(MAN)

$(TARGET): $(OBJS)

uninstall:
	rm -f $(BIN)/$(TARGET)
	rm -f $(MAN)/$(TARGET).1

clean:
	rm -f *.o $(TARGET) *core*
	rm -fr testdata

testdata:
	if [ ! -d testdata ]; then	\
		mkdir testdata ;	\
	fi ;				\
	for i in $(seq 1 100000) ;	\
	do				\
		touch testdata/test$i;	\
	done

.PHONY: all clean install uninstall testdata

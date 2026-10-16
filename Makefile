PROG=           minifetch

PREFIX?=        /usr/local
BINDIR=         ${PREFIX}/bin
DATADIR=        ${PREFIX}/share/minifetch
DESTDIR?=

SRCS=           src/main.c \
                src/system.c \
                src/memory.c \
                src/cpu.c \
                src/load.c \
                src/filesystem.c \
                src/ui.c \
                src/display.c

OBJS=           ${SRCS:.c=.o}

CFLAGS+=        -Wall -Wextra
CFLAGS+=        -Isrc
CFLAGS+=        -DMF_DATADIR=\"${DATADIR}\"

all: ${PROG}

${PROG}: ${OBJS}
	${CC} ${CFLAGS} -o ${PROG} ${OBJS}

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

install: ${PROG}
	mkdir -p ${DESTDIR}${BINDIR}
	mkdir -p ${DESTDIR}${DATADIR}
	install -m 755 ${PROG} ${DESTDIR}${BINDIR}/${PROG}
	install -m 644 artwork/minix.ascii ${DESTDIR}${DATADIR}/minix.ascii

uninstall:
	rm -f ${DESTDIR}${BINDIR}/${PROG}
	rm -f ${DESTDIR}${DATADIR}/minix.ascii

clean:
	rm -f ${PROG} ${OBJS}

.PHONY: all install uninstall clean

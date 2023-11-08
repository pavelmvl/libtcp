
SOURCE := $(wildcard src/*.c)
OBJECT := $(SOURCE:.c=.o)

CC     := gcc
CFLAGS := -Wall -I include
LDFLAG := -lpthread

all: lib/libtcp.a lib/libtcp.so

lib/libtcp.a: ${OBJECT}
	mkdir -p lib
	ar rcs "$@" $^

lib/libtcp.so: ${OBJECT}
	mkdir -p lib
	${CC} -shared -o "$@" $<

%.o:%.c
	${CC} ${CFLAGS} -o "$@" -c "$<"

clean:
	${RM} ${OBJECT}
	${RM} lib/libtcp.a
	${RM} lib/libtcp.so
	${RM} -r lib/

CC=g++ -g -Wall -std=c++17

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC+=-D_XOPEN_SOURCE -Wno-deprecated-declarations
    LIBCPU=libcpu_macos.o
else
    LIBCPU=libcpu.o
endif

# List of source files for your thread library
THREAD_SOURCES=cpu.cpp cv.cpp mutex.cpp thread.cpp

# Generate the names of the thread library's object files
THREAD_OBJS=${THREAD_SOURCES:.cpp=.o}

all: libthread.o test2 test5 test6 test110 test111 test205 test206 test210 test211 test212 test213 test214 test301 test302 test303 test304 test305 test306 test307 test308 test601 test602 test701 test702 test703 test704

# Compile the thread library and tag this compilation
libthread.o: ${THREAD_OBJS}
	./autotag.sh push
	ld -r -o $@ ${THREAD_OBJS}

# Compile an application program

test2: test2.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test5: test5.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test6: test6.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test110: test110.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test111: test111.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test205: test205.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test206: test206.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test210: test210.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test211: test211.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test212: test212.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test213: test213.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test214: test214.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test301: test301.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test302: test302.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test303: test303.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test304: test304.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test305: test305.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test306: test306.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test307: test307.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test308: test308.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test601: test601.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test602: test602.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test701: test701.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test702: test702.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test703: test703.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

test704: test704.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${THREAD_OBJS} libthread.o

	rm -f  test2 test3  test5 test6 
	rm -rf  test2.dSYM test3.dSYM test5.dSYM test6.dSYM

	rm -f test110 test111
	rm -rf test110.dSYM test111.dSYM
	
	rm -f test205 test206 test210 test211 test212 test213 test214
	rm -rf test205.dSYM test206.dSYM test210.dSYM test211.dSYM test212.dSYM test213.dSYM test214.dSYM

	rm -f test301 test302 test303 test304 test305 test306 test307 test308
	rm -rf test301.dSYM test302.dSYM test303.dSYM test304.dSYM test305.dSYM test306.dSYM test307.dDSYM test308.dDSYM

	rm -f test601 test602
	rm -rf test601.dSYM test602.dSYM

	rm -f test701 test702 test703 test704
	rm -rf test701.dSYM test702.dSYM test703.dSYM test704.dSYM
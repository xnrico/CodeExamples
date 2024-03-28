CC=g++ -g -Wall -fno-builtin -std=c++17

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC+=-D_XOPEN_SOURCE
    LIBVMAPP=libvm_app_macos.o
    LIBVMPAGER=libvm_pager_macos.o
else
    LIBVMAPP=libvm_app.o
    LIBVMPAGER=libvm_pager.o
endif

# List of source files for your pager
PAGER_SOURCES= vm_pager.cpp

# Generate the names of the pager's object files
PAGER_OBJS=${PAGER_SOURCES:.cpp=.o}

all: pager test1 test100

# Compile the pager and tag this compilation
pager: ${PAGER_OBJS} ${LIBVMPAGER}
	./autotag.sh push
	${CC} -o $@ $^

# Compile an application program
test1: test1.cpp ${LIBVMAPP}
	${CC} -o $@ $^ -ldl

# Compile an test program
test100: test100.cpp ${LIBVMAPP}
	${CC} -o $@ $^ -ldl

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${PAGER_OBJS} pager test1 test100

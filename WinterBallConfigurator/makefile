IGNORE_ISO = -Wno-writable-strings

POLLY_OPTS =

ifdef polly
	POLLY_OPTS = -mllvm -polly
endif

# Linux
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)

build:
	clang++ -std=c++20 -o bin/ballConfigGenerator src/main.cpp lib/libraylib.a -Iinclude -L/usr/lib -Llib -ldl -pthreads -O3 $(IGNORE_ISO) $(POLLY_OPTS)

run:
	./bin/ballConfigGenerator

test: build run

gdb:
	cp -Rf assets bin/
	clang++ -std=c++20 -o bin/ballConfigGenerator src/main.cpp lib/libraylib.a -Iinclude -L/usr/lib -Llib -ldl -pthreads -O3 $(IGNORE_ISO) $(POLLY_OPTS) -g
	gdb -ex run bin/ballConfigGenerator

endif

# mac
ifeq ($(UNAME_S),Darwin)
	@echo "Unsupported operating system due to my lack of mac devices, feel free to make a pr with a working build system"
endif
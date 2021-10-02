CORES := $(shell nproc)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
.PHONY: build run gdb clean

build:
	@make build -j$(CORES) -f makefile_real $@

run:
	@make run -j$(CORES) -f makefile_real $@

test:
	@make test -j$(CORES) -f makefile_real $@

gdb:
	@make gdb -j$(CORES) -f makefile_real $@

clean:
	@make clean -j$(CORES) -f makefile_real $@

endif

# mac
ifeq ($(UNAME_S),Darwin)
	@echo "Unsupported operating system due to my lack of mac devices, feel free to make a pr with a working build system"
endif
SRC_DIR := src
OBJ_DIR := src
C_SRC_FILES := $(filter-out src/raylib/src/external,$(shell find $(SRC_DIR)/ -path src/raylib/src/external -prune -o -type f -name '*.c'))
CXX_SRC_FILES := $(filter-out src/raylib/src/external,$(shell find $(SRC_DIR)/ -path src/raylib/src/external -prune -o -type f -name '*.cpp') $(shell find $(SRC_DIR)/ -path src/raylib/src/external -prune -o -type f -name '*.cc'))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CXX_SRC_FILES))) $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SRC_FILES))

INCLUDE_DIRS = -Iinclude \
				-Isrc/raylib/src \
				-Isrc/raylib/extras \
				-Isrc/raylib/src/external/glfw \
				-Isrc/raylib/src/external/glfw/include \
				-Isrc/raylib/src/external/glfw/include/GLFW \
				-Isrc/raylib/src/external/glfw/deps \
				-Isrc/raylib/src/external/glfw/deps/glad \
				-Isrc/raylib/src/external/glfw/deps/mingw \
				-Isrc/raylib/src/external/glfw/src/
LINK_LIBRARIES = -L/usr/lib -Llib -ldl
IGNORE_ISO_STANDARD = -Wno-writable-strings -Wno-missing-braces -Werror=pointer-arith -fno-strict-aliasing

POLLY_OPTS =
ifdef polly
	POLLY_OPTS = -mllvm -polly
endif

GLFW_DEFINES = 
ifeq ($(OS),Windows_NT)
	GLFW_DEFINES = -D_GLFW_WIN32=1
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        GLFW_DEFINES = -D_GLFW_X11=1
    endif
    ifeq ($(UNAME_S),Darwin)
        GLFW_DEFINES = -D_GLFW_COCOA=1
    endif
endif

GDB_OPTS = 

#  ____        _ _     _
# |  _ \      (_) |   | |
# | |_) |_   _ _| | __| |
# |  _ <| | | | | |/ _` |
# | |_) | |_| | | | (_| |
# |____/ \__,_|_|_|\__,_|
#

# Linux
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
.PHONY: build run gdb clean

build: $(OBJ_FILES)
	clang++ -std=c++20 -o bin/ballConfigGenerator $(OBJ_FILES) $(INCLUDE_DIRS) $(LINK_LIBRARIES) -pthreads -O3 $(IGNORE_ISO) $(POLLY_OPTS)

run:
	./bin/ballConfigGenerator

test: build run

gdb: setGdbOpts $(OBJ_FILES)
	clang++ -std=c++20 -o bin/ballConfigGenerator $(OBJ_FILES) $(INCLUDE_DIRS) $(LINK_LIBRARIES) -pthreads -O3 $(IGNORE_ISO) $(POLLY_OPTS) -g
	gdb -ex run bin/ballConfigGenerator

setGdbOpts:
	$(eval GDB_OPTS := -g)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	clang++ -std=c++20 -c -o $@ $< $(INCLUDE_DIRS) -pthreads -O3 $(IGNORE_ISO) -fPIC -D_DEFAULT_SOURCE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 $(POLLY_OPTS) $(GLFW_DEFINES) $(GDB_OPTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo $(C_SRC_FILES)
	@echo $(CXX_SRC_FILES)
	@echo $(OBJ_FILES)
	clang -std=c99 -c -o $@ $< $(INCLUDE_DIRS) -pthreads -O3 $(IGNORE_ISO) -fPIC -D_DEFAULT_SOURCE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 $(POLLY_OPTS) $(GLFW_DEFINES) $(GDB_OPTS)

clean:
	rm -rf bin
	mkdir bin
	find . -type f -name '*.o' -delete

endif

# mac
ifeq ($(UNAME_S),Darwin)
	@echo "Unsupported operating system due to my lack of mac devices, feel free to make a pr with a working build system"
endif
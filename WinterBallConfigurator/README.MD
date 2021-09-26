# Configurator

A simple configuration genertor for the mod, it is also able to load and edit a config file.

- If you want to edit any of the images from a loaded config or gif you can edit `gifLoaderTempFiles/frame_<number>.png`.
- `gifLoaderTempFiles` is in the same folder you the program from.

## Compiling

Supported platforms:

- Linux using clang
  - To compile, run `make build`
  - To compile then run, run `make test`
  - To compile then run in gdb, run `make gdb`
  - To compile then run the latest build, run `make run`
  - If your installation of clang supports polly(if you didn't explicitly compile clang with it, it probably doesn't), you can use `make <command> polly=true` to enable it's optimizations while building
  - The current libraylib.a is x86, i will add an arm version when i get my pi working
- Windows using Visual studio (all architectures with VS support)
  - Simple, just open the .sln and click build

Unsupported platforms:

- Macos
  - I have no way to test this platform, feel free to make a PR
- FreeBSD
  - I have no way to test this platform, feel free to make a PR
- Raspberry Pi
  - Will be added at some point, feel free to make a PR
- Android
  - Why would you even be using this on your phone?
  - If for some god forsaken reason you *want* android support, feel free to make a PR
- Web
  - Some day might try to figure out doing this on web, until then you need a desktop
- Other platforms
  - If you want to see another platform, feel free to make a PR

## Bugs?

This is primarily developed on linux, you can open bug reports for any supported os but non-linux fixes may take longer, especially for platforms i have no way to fix.

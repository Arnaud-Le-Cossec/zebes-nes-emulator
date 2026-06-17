# ZEBES EMULATOR

[![Linux build & test](https://github.com/Arnaud-Le-Cossec/zebes-emulator/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/Arnaud-Le-Cossec/zebes-emulator/actions/workflows/cmake-linux.yml)

A zimple NES emulator, written in C

## Init & update submodules

```bash
git submodule update --init
```

## Build

1. Create a build directory:

   ```bash
   mkdir build && cd build
   ```

2. Run CMake to configure the project:

   ```bash
   cmake -S . -B ./build
   ```

3. Build the project:

   ```bash
   cmake --build ./build
   ```
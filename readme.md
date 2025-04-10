# CSE 111 Emulator

## How to Build and Run

In the root of a freshly cloned repo run:

```sh
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make -j$(nproc)
```

This will create an executable `emulator` in the current directory.

### Build Options

You can change the behavior of the emulator by passing `-DCMAKE_BUILD_TYPE=<build_type>` to Cmake.

The `Release` option is passed by default, and the `Headless` option builds the emulator without a
GUI.

| Cmake Option        | Clang flags                    |
| ------------------- | ------------------------------ |
| `Debug`             | `-ggdb -O0`                    |
| `Release` (default) | `-O3 -Werror`                  |
| `Headless`          | `-O3 -Werror -DHEADLESS_BUILD` |

## Team Members

- Ryan Welter
- Wyatt Avilla
- Monisha Garika
- Michelle Gurovith
- Michael Kamensky


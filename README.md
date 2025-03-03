# Bezier Curves in C using SDL

![thumbnail](docs/thumbnail.png)

## Overview

This project is an experiment to create and visualize **Bezier Curves** using **C** and the **SDL2** library. The implementation allows rendering smooth curves using a set of control points.

## Features

- Render Bezier curves with handles, just like in tools like Figma, Inkscape, Adobe Illustrator, etc.
- Interactive control points.
- Control points are interconnected to display initial path in conjunction with final result.
- Uses SDL2 for rendering.

## Prerequisites

Ensure you have the following installed before compiling the project:

- **GCC or Clang** (C compiler)
- **SDL2 library**
- **Make** (optional, for using the Makefile)

### Installing SDL2

On Fedora:

```sh
sudo dnf install SDL2 SDL2-devel
```

On Debian/Ubuntu:

```sh
sudo apt update
sudo apt install libsdl2-dev
```

On Windows (MSYS2 MinGW64):

```sh
pacman -S mingw-w64-ucrt-x86_64-SDL2
```

## Building the Project

### Using Makefile

To compile the project and output the executable to the `build/` directory, run:

```sh
make
```

### Manually Compiling

If not using Makefile, compile with:

```sh
gcc -Wall -Wextra -std=c11 -pedantic `pkg-config --cflags --libs sdl2` -o build/bezier src/main.c -lm
```

## Running the Program

After compiling, run the executable:

```sh
./build/bezier
```

## Cleaning the Build

To remove the compiled files and clean the project:

```sh
make clean
```

## File Structure

```
bezier-curves/
├── build/                # Compiled output directory
│   ├── bezier            # Linux executable
│   ├── bezier.exe        # Windows executable (if cross-compiled)
│
├── docs/                 # Compiled output directory
│   ├── thumbnail.png     # Project image
│
├── src/
│   ├── main.c            # Main file for rendering Bezier curves
│
├── LICENSE               # License file
├── Makefile              # Compilation rules
├── README.md             # Documentation
```

## Bezier Curve Basics

- **Quadratic Bezier Curve**: Uses 3 points (P0, P1, P2)
- **Cubic Bezier Curve**: Uses 4 points (P0, P1, P2, P3) and so on

- Formula:

  ```
  Quadratic - B(t) = (1-t)^2 * P0 + 2(1-t)t * P1 + t^2 * P2
  Cubic - B(t) = (1-t)^3 * P0 + 3(1-t)^2t * P1 + 3(1-t)t^2 * P2 + t^3 * P3
  ```

- This implementation supports **Arbitrary Degree Bezier Curves**.

## License

This project is released under the **MIT License**.

### Author

Anvesh Khode

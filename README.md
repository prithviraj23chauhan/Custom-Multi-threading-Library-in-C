# Custom Multi-threading Library in C

## Overview
This project implements a custom user-level multi-threading library in C, providing basic thread management, synchronization, and scheduling features. It is designed for educational purposes and demonstrates how threading can be implemented without relying on OS-level threads.

## Features
- Create threads with or without arguments
- Cooperative scheduling (yield, join, suspend, resume)
- Thread synchronization (lock, release)
- Thread return values
- Simple round-robin scheduler

## Files
- `Mythreads.c` / `Mythreads.h`: The threading library implementation and API
- `Test_Mytherads.c`: Example/test program using the library
- `Makefile`: Build instructions

## Build & Run
To build and run the test program:
```sh
make
./test
```

To clean up build artifacts:
```sh
make clean
```

## Usage Example
See `Test_Mytherads.c` for example usage. Typical workflow:
```c
#include "Mythreads.h"

void myfunc() { /* ... */ }

int main() {
    create(myfunc);
    start();
    return 0;
}
```

## API Reference
See `Mythreads.h` for function documentation and usage details.


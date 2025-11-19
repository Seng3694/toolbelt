# Toolbelt

A lightweight collection of single-header C99 utility libraries for easy integration into your projects. Each file is self-contained and documented for drop-in use.


## Current Modules

| Header       | Description |
|--------------|-------------|
| [deque.h](src/deque.h) | Double ended queue |
| [map.h](src/map.h) | Hashmap/Hashset |
| [arena.h](src/arena.h) | Arena allocator |

## Quick Start

Copy the header you want to use from the `src` directory into your project or clone the whole repository.

Defining and implementing the types and functions in a single source file:

```c
// main.c
#define TLBT_T int
#define TLBT_STATIC
#include "deque.h"

int main(void) {
  // deque type and functions defined now
  tlbt_deque_int d = {0};
  // see deque.h and tests for usage
}
```

Defining and implementing in separate files for reuse:

header file:

```c
// my_deque.h
#pragma once

#define TLBT_T int
#define TLBT_DEFINITION
#include "deque.h"
```

source file:

```c
// my_deque.c
#include "my_deque.h"
#define TLBT_T int
#define TLBT_IMPLEMENTATION
#include "deque.h"
```

Now you can use your `my_deque.h` everywhere in your project. Make sure to compile the source file and link it with your executable
```c
// main.c
#include "my_deque.h"

int main(void) {
  tlbt_deque_int d = {0};
}
```

For more detailed usage please check the documentation inside each header or their corresponding tests in the `test` directory.


## Tests 

When cloning the repository you can also run the tests with `make test` to run all of them or `make build/deque_default && ./build/deque_default` to run a specific one.


## Planned Modules

| Header       | Description |
|--------------|-------------|
| heap.h | Min/Max heap type |
| array.h | Regular array type |
| filesystem.h | File utilities |


## License

This software is licensed under the MIT License. See [LICENSE](LICENSE) for more information.


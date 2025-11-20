# Toolbelt

A lightweight collection of single-header C99 utility libraries for easy integration into your projects. Each file is self-contained and documented for drop-in use.


## Current Modules

| Header       | Description | Template Header |
|--------------|-------------|-----------------|
| [deque.h](src/deque.h) | Double ended queue | yes |
| [map.h](src/map.h) | Hashmap/Hashset | yes |
| [arena.h](src/arena.h) | Arena allocator | no |
| [heap.h](src/heap.h) | Min/Max heap | yes |

## Quick Start

Copy the header you want to use from the `src` directory into your project or clone the whole repository. There are two different kinds of header files:

### Regular header files

```c
// main.c
// define this macro once in a source file
#define TLBT_IMPLEMENTATION
#include "arena.h"

int main(void) {
  // tlbt_arena type and functions are defined and implemented
  tlbt_arena a = {0};
  // see arena.h and tests for usage
}
```

### Template header files

You can define the type, functions and behavior yourself with macros. Define the macros before including the file. These header files can be included as often as you want because they are for defining your own types rather than providing them.

Defining and implementing the types and functions in a single source file:

```c
// main.c
#define TLBT_T int
#define TLBT_STATIC
#include "deque.h"

// it's completely fine to include it again with a different type parameter
#define TLBT_T char*
// necessary because "char*" can't be used in the name due to the asterisk
// you can also wrap the pointer in a struct or typedef it
#define TLBT_T_NAME str
#define TLBT_STATIC
#include "deque.h"

int main(void) {
  // tlbt_deque_int/str type and functions are only defined in this file
  tlbt_deque_int d1 = {0};
  tlbt_deque_str d2 = {0};
  // see deque.h and tests for usage
}
```

If you want to reuse your defined type, you should not include them statically in every source file but define and implement them in separate files:

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
| array.h | Regular array type |
| filesystem.h | File utilities |


## License

This software is licensed under the MIT License. See [LICENSE](LICENSE) for more information.


## Documentation

This is a header-only library, as such most of its functional documentation is contained within the "header section" of the
source code in the form of comments. It is highly recommended that you read said documentation before using this library.

## Features

The dynarr library provides type-generic general-purpose array data structures, its features include:

- No hard dependencies besides the standard library, making it fully portable for most purposes
- Configurable memory allocation function, allowing for compatibility with custom memory managers
- Usable as a stack, queue, dynamic array, binary-searchable list, or all of the above at once
- Supports sorting using either its own linear sort or the standard qsort implemenetation

## Example

```c
#define DYNARR_STATIC //static version
#include "dynarr.h" //include dynarr
#include <stdio.h> //printf

static int sortInt(const int*, const int*);

int main () {
    //declare and initialize dynarr
    int* darr = DYNARR_NEW(int);
    //push a couple of elements
    DYNARR_PUSH(darr, 4);
    DYNARR_PUSH(darr, 1);
    DYNARR_PUSH(darr, 3);
    DYNARR_PUSH(darr, 2);
    //sort dynarr in ascending order
    DYNARR_SORT_STD(darr, sortInt);
    //iterate through the dynarr and print its elements
    for (int i = 0; i < DYNARR_SIZE(darr); i++)
        printf("darr[%d] = %d\n", i, DYNARR_AT(darr, i));
    //free the dynarr
    DYNARR_FREE(darr);
    //return
    return 0;
}

static int sortInt (const int* a, const int* b) {
    return *a-*b; //ascending order
}
```

## Attribution

You are not required to give attribution when using this library. If you want to give attribution anyway, either link to
this repository, [my website](https://www.slopegames.com/), or credit me as [BareRose](https://github.com/BareRose).
If you want to support me financially, consider giving to my [Patreon](https://www.patreon.com/slopegames).

## License

Licensed under CC0 aka the most lawyer-friendly way of spelling "public domain".
/*
The MIT License (MIT)

Copyright (c) 2015 Julian Ingram

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#ifndef EMEDY_H
#define	EMEDY_H

#include <stdlib.h>

// converts a size in chars to a size in `struct emedy_header**`s, always rounds
// up
#define emedy_cth(size) ((size + (sizeof(struct emedy_header**) - 1)) \
/ sizeof(struct emedy_header**))

// used to declare a section of memeory for use by emedy, pass `name` into the
// emedy_allocate and emedy_reallocate functions as the `nh` argument. this will
// allocate an array large enough to fit `size` chars, but its length will be
// aligned to `sizeof(struct emedy_header**)`
#define emedy_heap(name, size) struct emedy_header** name[emedy_cth(size)] \
= {(struct emedy_header**) (nh + (emedy_cth(size) - 1)), 0}

// the first next value points to a NULL pointer. The last `next` value should
// always point to a 0 the first previous value should also be 0

struct emedy_header
{
    struct emedy_header* next;
    struct emedy_header* previous;
    unsigned char allocated;
};

void* emedy_allocate(void* const nh, const size_t size);
void emedy_deallocate(void* const ptr);
void* emedy_reallocate(void* const nh, void* const ptr, const size_t new_size);

#endif

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

// Emedy is a simple memory allocator for embedded applications where memory is
// tight or a malloc/realloc implementation does not exist.  

#include "emedy.h"

static inline size_t emedy_get_size(const struct emedy_header * const nh)
{
    return (((char*) nh->next) - ((char *) nh)) - sizeof (*nh);
}

struct emedy_best_fit
{
    struct emedy_header* header;
    size_t size;
};

static inline struct emedy_best_fit emedy_find_header(const void* const nh,
                                                      const size_t size)
{
    struct emedy_best_fit best_fit = {0, (size_t) - 1};
    if (size != 0)
    {
        struct emedy_header* nc = (struct emedy_header *) nh;
        while (nc->next != 0)
        { // best fit
            size_t section_size = emedy_get_size(nc);
            if ((nc->allocated == 0) && (section_size >= size)
                    && (section_size < best_fit.size))
            {
                best_fit.header = nc;
                best_fit.size = section_size;
            }
            nc = nc->next;
        }
    }
    return best_fit;
}

static inline void emedy_allocate_section(struct emedy_best_fit best_fit,
                                          const size_t size)
{
    if (best_fit.size > (size + sizeof (*best_fit.header)))
    { // a section split is required
        struct emedy_header * const new_next
                = (struct emedy_header * const) (((char*) best_fit.header)
                + sizeof (*best_fit.header) + size);
        new_next->allocated = 0;
        new_next->next = best_fit.header->next;
        new_next->previous = best_fit.header;
        new_next->next->previous = new_next;
        best_fit.header->next = new_next;
    }
    best_fit.header->allocated = 1;
}

void* emedy_allocate(void* const nh, const size_t size)
{
    struct emedy_best_fit best_fit = emedy_find_header(nh, size);
    if (best_fit.header != 0) // space found
    {
        emedy_allocate_section(best_fit, size);
        return best_fit.header + 1;
    }
    return 0;
}

static inline void emedy_deallocate_from_header(struct emedy_header * const nc)
{
    if ((nc->next != 0) && (nc->next->allocated == 0))
    { // merge next with current
        nc->next = nc->next->next;
        if (nc->next != 0)
        {
            nc->next->previous = nc;
        }
    }
    if ((nc->previous != 0) && (nc->previous->allocated == 0))
    { // merge previous with current
        nc->previous->next = nc->next;
        nc->next->previous = nc->previous;
    }
    else
    {
        nc->allocated = 0;
    }
}

void emedy_deallocate(void* const ptr)
{
    struct emedy_header * const nc = ((struct emedy_header * const) ptr) - 1;
    emedy_deallocate_from_header(nc);
}

// may relocate the memory even when the size is reduced.

void* emedy_reallocate(void* const nh, void* ptr, const size_t size)
{
    if (ptr == 0)
    {
        return emedy_allocate(nh, size);
    }
    else
    {
        struct emedy_header * const nc = ((struct emedy_header * const)
                ptr) - 1;
        emedy_deallocate_from_header(nc);
        struct emedy_best_fit best_fit = emedy_find_header(nh, size);
        if (best_fit.header != 0) // space found
        {
            if (best_fit.header != nc)
            { // copy req'd
                const size_t old_size = emedy_get_size(nc);
                const size_t smallest_size = (size < old_size) ? size
                        : old_size;
                char* new_idx = (char*) (best_fit.header + 1);
                const char* const new_lim = new_idx + smallest_size;
                while (new_idx < new_lim)
                {
                    *new_idx = *((char*) ptr);
                    ++new_idx;
                    ++ptr;
                }
            }
            emedy_allocate_section(best_fit, size);
            return best_fit.header + 1;
        }
    }
    return 0;
}

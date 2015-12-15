# Emedy #

A simple memory allocator.

`emedy_heap(name, size)`

A macro used to define a section of memory for use by emedy, pass `name` into the `emedy_allocate` and `emedy_reallocate` functions as the `nh` argument.

It's worth taking a look at what this macro does in the header file, it's not the cleanest use of the preprocessor I have ever come up with and there may be some compatibility issues with surrounding code if it is placed without knowing what it will be replaced with.

`void* emedy_allocate(void* const nh, const size_t size);`

Roughly equivalent to `malloc`. Allocates a section of memory of size `size` from that allocated by the `emedy_heap` macro (or otherwise), the "emedy heap".

`void emedy_deallocate(void* const ptr);`

Roughly equivalent to `free`, frees a section of memory within the "emedy heap".

`void* emedy_reallocate(void* const nh, void* const ptr, const size_t new_size);`

Roughly equivalent to `realloc`, resizes or allocates (if `ptr == NULL`) a section of memory of size `size` from the "emedy heap". Note that this may relocate the memory section even if the size is reduced as a "best fit" algorithm is used.

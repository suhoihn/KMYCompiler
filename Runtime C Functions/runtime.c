#include <stdio.h>
#include <stdint.h>

// gcc -c runtime.c -o runtime.o
void runtime_0(int64_t x) {
    printf("runtime got: %lld\n", x);
    
    // Windows sometimes buffers standard output until the program 
    // cleanly exits. For compiler dev, force it to print immediately:
    fflush(stdout); 
}
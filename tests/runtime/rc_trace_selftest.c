/* Calibrate the trace checker itself: good ownership, a leak, and double release. */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef void (*KMYDestructor)(void *value);
void *kmy_rc_alloc(int64_t size, KMYDestructor destructor);
void kmy_rc_retain(void *payload);
void kmy_rc_release(void *payload);

int main(int argc, char **argv) {
    if (argc != 2) return 2;
    void *value = kmy_rc_alloc(8, NULL);
    if (strcmp(argv[1], "leak") == 0) return 0; /* Trace must reject live=1. */
    if (strcmp(argv[1], "double-release") == 0) {
        kmy_rc_release(value);
        kmy_rc_release(value); /* Trace must reject before touching freed memory. */
        return 0;
    }
    if (strcmp(argv[1], "balanced") != 0) return 2;
    kmy_rc_retain(value);
    kmy_rc_release(value);
    kmy_rc_release(value);
    return 0;
}

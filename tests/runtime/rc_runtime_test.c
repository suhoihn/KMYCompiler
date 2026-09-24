#include <assert.h>
#include <stdint.h>
#include <stddef.h>

typedef void (*KMYDestructor)(void *value);

void *kmy_rc_alloc(int64_t payloadSize, KMYDestructor destructor);
void kmy_rc_retain(void *value);
void kmy_rc_release(void *value);

static int destructorCalls = 0;

// Confirms that destruction runs once and can still inspect the live payload.
static void test_destructor(void *value) {
    int64_t *payload = value;
    assert(*payload == 42);
    destructorCalls++;
}

int main(void) {
    // Null handles are valid no-ops for nullable shared values.
    kmy_rc_retain(NULL);
    kmy_rc_release(NULL);

    // Invalid negative sizes fail without attempting an allocation.
    assert(kmy_rc_alloc(-1, NULL) == NULL);

    int64_t *value = kmy_rc_alloc(sizeof(int64_t), test_destructor);
    assert(value != NULL);

    // RC payload storage is intentionally uninitialized, like malloc. Assign
    // before reading it; constructors/initializers own payload initialization.
    *value = 42;

    kmy_rc_retain(value);       // strong count: 1 -> 2
    kmy_rc_release(value);      // strong count: 2 -> 1
    assert(destructorCalls == 0);

    kmy_rc_release(value);      // strong count: 1 -> 0, then destroy
    assert(destructorCalls == 1);

    // `value` is dangling after the final release and must not be read again.
    return 0;
}

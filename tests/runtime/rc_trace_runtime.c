/* Test-only runtime: wrap the real implementation, not a replacement RC model.
 * Build this file INSTEAD OF runtime.c/runtime.o. Stable IDs make traces comparable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

static void rc_test_free(void *pointer);
#define kmy_rc_alloc rc_test_real_alloc
#define kmy_rc_retain rc_test_real_retain
#define kmy_rc_release rc_test_real_release
#define free rc_test_free
#include "../../Runtime C Functions/runtime.c"
#undef free
#undef kmy_rc_alloc
#undef kmy_rc_retain
#undef kmy_rc_release

typedef struct RcTestAllocation {
    void *payload;
    int live;
} RcTestAllocation;

/* Small tests only; fail explicitly rather than allocating unbounded bookkeeping. */
static RcTestAllocation allocations[256];
static int allocationCount, retainCount, releaseCount, freeCount;
static int registeredSummary;

static void rc_test_fail(const char *message) {
    fprintf(stderr, "RC ERROR %s\n", message);
    exit(97);
}

static void rc_test_summary(void) {
    int live = 0;
    for (int i = 0; i < allocationCount; ++i) live += allocations[i].live;
    fprintf(stderr, "RC summary alloc=%d retain=%d release=%d free=%d live=%d\n",
        allocationCount, retainCount, releaseCount, freeCount, live);
    if (live != 0) {
        fputs("RC ERROR managed allocations leaked\n", stderr);
        /* Do not call exit recursively from an atexit handler. */
        fflush(stderr);
        _exit(98);
    }
}

static int rc_test_find(void *value) {
    /* Search newest first: malloc may reuse a previously freed address. */
    for (int i = allocationCount - 1; i >= 0; --i) {
        if (allocations[i].payload == value && allocations[i].live) return i;
    }
    rc_test_fail("retain/release of unknown or already freed payload");
    return -1;
}

static void rc_test_free(void *pointer) {
    for (int i = allocationCount - 1; i >= 0; --i) {
        if (allocations[i].live && pointer == (KMYRcHeader *)allocations[i].payload - 1) {
            allocations[i].live = 0;
            ++freeCount;
            fprintf(stderr, "RC free id=%d\n", i + 1);
            break;
        }
    }
    free(pointer); /* Actual C allocator free, not just a counter update. */
}

void *kmy_rc_alloc(int64_t size, KMYDestructor destructor) {
    if (!registeredSummary) {
        if (atexit(rc_test_summary) != 0) rc_test_fail("could not register summary");
        registeredSummary = 1;
    }
    if (allocationCount == 256) rc_test_fail("test allocation limit exceeded");
    void *payload = rc_test_real_alloc(size, destructor);
    if (!payload) rc_test_fail("allocation failed");
    allocations[allocationCount++] = (RcTestAllocation){payload, 1};
    const int64_t count = kmy_rc_header(payload)->strongCount;
    fprintf(stderr, "RC alloc id=%d count=%" PRId64 "\n", allocationCount, count);
    if (count != 1) rc_test_fail("allocation did not start with one owner");
    return payload;
}

void kmy_rc_retain(void *payload) {
    if (!payload) { rc_test_real_retain(NULL); return; }
    const int id = rc_test_find(payload);
    const int64_t before = kmy_rc_header(payload)->strongCount;
    rc_test_real_retain(payload);
    const int64_t after = kmy_rc_header(payload)->strongCount;
    ++retainCount;
    fprintf(stderr, "RC retain id=%d count=%" PRId64 "\n", id + 1, after);
    if (after != before + 1) rc_test_fail("retain did not add one owner");
}

void kmy_rc_release(void *payload) {
    if (!payload) { rc_test_real_release(NULL); return; }
    const int id = rc_test_find(payload);
    const int64_t before = kmy_rc_header(payload)->strongCount;
    ++releaseCount;
    fprintf(stderr, "RC release id=%d count=%" PRId64 "\n", id + 1, before - 1);
    rc_test_real_release(payload);
    if (before == 1) {
        /* Do not read the header after the final release; it is freed memory. */
        if (allocations[id].live) rc_test_fail("final release did not free allocation");
    } else {
        if (!allocations[id].live) rc_test_fail("allocation freed before its last owner");
        if (kmy_rc_header(payload)->strongCount != before - 1) {
            rc_test_fail("release did not remove one owner");
        }
    }
}

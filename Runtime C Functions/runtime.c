#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// gcc -c runtime.c -o runtime.o
void runtime_0(int64_t x) {
    printf("%lld\n", x);
    
    // Windows sometimes buffers standard output until the program 
    // cleanly exits. For compiler dev, force it to print immediately:
    fflush(stdout); 
}

void runtime_1(const char *value) {
    printf("%s\n", value ? value : "(null)");
    fflush(stdout);
}

int64_t runtime_2(const char *left, const char *right) {
    if (left == NULL || right == NULL) return left == right;
    return strcmp(left, right) == 0;
}

const char *runtime_3(const char *left, const char *right) {
    if (left == NULL) left = "";
    if (right == NULL) right = "";

    size_t leftLen = strlen(left);
    size_t rightLen = strlen(right);
    char *result = malloc(leftLen + rightLen + 1);
    if (result == NULL) return NULL;

    memcpy(result, left, leftLen);
    memcpy(result + leftLen, right, rightLen);
    result[leftLen + rightLen] = '\0';
    return result;
}

int64_t runtime_4(const char *value) {
    return value ? (int64_t)strlen(value) : 0;
}

int64_t runtime_5(const char *value, int64_t index) {
    if (value == NULL || index < 0 || (size_t)index >= strlen(value)) {
        return -1;
    }
    return (unsigned char)value[index];
}

char *runtime_8(int64_t value) {
    char *result = malloc(2);
    if (result == NULL) return NULL;
    if (value < 0 || value > 255) {
        result[0] = '\0';
    } else {
        result[0] = (char)(unsigned char)value;
        result[1] = '\0';
    }
    return result;
}

char *runtime_6(const char *path) {
    if (path == NULL) return NULL;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return NULL;

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    long size = ftell(file);
    if (size < 0 || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    char *contents = malloc((size_t)size + 1);
    if (contents == NULL) {
        fclose(file);
        return NULL;
    }

    size_t readCount = fread(contents, 1, (size_t)size, file);
    fclose(file);
    if (readCount != (size_t)size) {
        free(contents);
        return NULL;
    }
    contents[size] = '\0';
    return contents;
}

int64_t runtime_7(const char *path, const char *contents) {
    if (path == NULL || contents == NULL) return 0;
    FILE *file = fopen(path, "wb");
    if (file == NULL) return 0;
    size_t length = strlen(contents);
    size_t written = fwrite(contents, 1, length, file);
    int closeResult = fclose(file);
    return written == length && closeResult == 0;
}

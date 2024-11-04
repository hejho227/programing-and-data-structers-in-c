#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EPSILON 0.0000000000000000000000001

typedef struct {
    int first;
    int second;
} IntPair;

void *bsearch(const void *key, const void *base, size_t num, size_t size,
              int (*compar)(const void *, const void *)) {
    const char *base_ptr = (const char *)base;
    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_ptr = base_ptr + (mid * size);
        int cmp_result = compar(key, mid_ptr);

        if (cmp_result == 0) {
            return (void *)mid_ptr;
        } else if (cmp_result < 0) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    return NULL;
}

int compareInt(const void *a, const void *b) {
    int intA = *((const int *)a);
    int intB = *((const int *)b);
    return (intA > intB) - (intA < intB); 
}

int compareFloat(const void *a, const void *b) {
    float floatA = *((const int *)a);
    float floatB = *((const int *)b);
    return (floatA - floatB > EPSILON) - (floatB - floatA > EPSILON); 
}

int compareIntPair(const void *a, const void *b) {
    const IntPair *pairA = (const IntPair *)a;
    const IntPair *pairB = (const IntPair *)b;
    if (pairA->first != pairB->first) {
        return (pairA->first > pairB->first) - (pairA->first < pairB->first);
    } else {
        return (pairA->second > pairB->second) - (pairA->second < pairB->second);
    }
}

int compareString(const void *a, const void *b) {
    const char *strA = *((const char **)a);
    const char *strB = *((const char **)b);
    return strcmp(strA, strB);
}

void testIntBeg() {
    int sorted_array[] = {-1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int key = -1;

    int *result = bsearch(&key, sorted_array, sizeof(sorted_array) / sizeof(sorted_array[0]), sizeof(int), compareInt);

    if (result != NULL) {
        printf("Found %d at index %ld\n", key, (result - sorted_array));
    } else {
        printf("%d not found\n", key);
    }
}
void testIntMid() {
    int sorted_array[] = {-1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int key = 5;

    int *result = bsearch(&key, sorted_array, sizeof(sorted_array) / sizeof(sorted_array[0]), sizeof(int), compareInt);

    if (result != NULL) {
        printf("Found %d at index %ld\n", key, (result - sorted_array));
    } else {
        printf("%d not found\n", key);
    }
}

void testFloat() {
    float sorted_array[] = {1.1, 2.02, 3.1, 4.5, 5.7, 6.2, 7.0, 8.1, 9.2, 10.0};
    float key = 1.1;

    float *result = bsearch(&key, sorted_array, sizeof(sorted_array) / sizeof(sorted_array[0]), sizeof(float), compareFloat);

    if (result != NULL) {
        printf("Found %lf at index %ld\n", key, (result - sorted_array));
    } else {
        printf("%lf not found\n", key);
    }
}

void testIntPairSearch() {
    IntPair sorted_pairs[] = {{1, 1}, {3, 10}, {3, 30}, {4, 31}, {5, 94}};
    IntPair key = {3, 30};

    IntPair *result = bsearch(&key, sorted_pairs, sizeof(sorted_pairs) / sizeof(sorted_pairs[0]), sizeof(IntPair), compareIntPair);

    if (result != NULL) {
        printf("Found pair (%d, %d)\n", result->first, result->second);
    } else {
        printf("Pair (%d, %d) not found\n", key.first, key.second);
    }
}

void testStringSearch() {
    const char *sorted_strings[] = {"andrzej", "bonifacy", "pomidor", "pomidorek", "galgan", "witold"};
    const char *key = "pomighdfgdorek";

    char **result = bsearch(&key, sorted_strings, sizeof(sorted_strings) / sizeof(sorted_strings[0]), sizeof(char *), compareString);

    if (result != NULL) {
        printf("Found string \"%s\"\n", *result);
    } else {
        printf("String \"%s\" not found\n", key);
    }
}
    
void testChar() {
    const char *sorted_strings[] = {"a", "b", "c", "p", "g", "w"};
    const char *key = "p";

    char **result = bsearch(&key, sorted_strings, sizeof(sorted_strings) / sizeof(sorted_strings[0]), sizeof(char *), compareString);

    if (result != NULL) {
        printf("Found char \"%s\"\n", *result);
    } else {
        printf("Char \"%s\" not found\n", key);
    }
}

int main() {
    testIntBeg();
    testIntMid();
    testFloat();
    testIntPairSearch();
    testChar();
    testStringSearch();
    return 0;
}

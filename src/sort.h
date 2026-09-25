#ifndef SORT_H
#define SORT_H
#include <stddef.h>

typedef int (*compare_fn)(const void* a, const void* b);

void quickSort(int start, int end, void *array, size_t elem_size, compare_fn cmp);

// NOTE: Need to add mergeSort to keep the original elements order in case two elements shared the same floats
void mergeSort(int start, int end, void* array, size_t elem_size, compare_fn cmp);
#endif

#include "sort.h"
#include <string.h>
#include <stdlib.h>

#define INSERTION_SORT_THRESHOLD 10

/////////////////////////////////////////////////////////////////////////////////
// Handler functions
/////////////////////////////////////////////////////////////////////////////////
// Unsigned char* is still the conventional choice for "raw bytes" pointers in C - concrete 1-byte type
static void* at(void* array, int idx, size_t elem_size) {
  return (unsigned char*)array + (size_t)idx * elem_size;
}

//Since we want to make it generic-alike, we won't know how many bytes we want the memeory allocate
static void swap_orders(void* a, void* b, size_t elem_size) {
  void* tmp = malloc(elem_size);
	memcpy(tmp, a, elem_size);
  memcpy(a, b, elem_size);
  memcpy(b, tmp, elem_size);
  free(tmp);
}
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// QuickSort
/////////////////////////////////////////////////////////////////////////////////
// Descending order: a > b > c
static void insertionSort(void* array, int start, int end, size_t elem_size, compare_fn cmp) {
  void* tmp = malloc(elem_size);
  for(int i = start + 1; i <= end; ++i) {
    memcpy(tmp, at(array, i, elem_size), elem_size);
    int j = i - 1;
    while(j >= start && cmp(at(array, j, elem_size), tmp) < 0) {
      memcpy(at(array, j + 1, elem_size), at(array, j, elem_size), elem_size);
      --j;
    }
    memcpy(at(array, j + 1, elem_size), tmp, elem_size);
  }
  free(tmp);
}

// Descending order: a > b > c, and we don't know for sure how to compare two objects
static void mediansOfThree(void* array, int start, int end, size_t elem_size, compare_fn cmp) {
  int middleIdx = start + (end - start) / 2;
  void* first = at(array, start, elem_size);
  void* mid = at(array, middleIdx, elem_size);
  void* last = at(array, end, elem_size);

  if(cmp(first, mid) < 0) {
    swap_orders(first, mid, elem_size);
  }
  if(cmp(mid, last) < 0) {
    swap_orders(mid, last, elem_size);
  }
  if(cmp(first, mid) < 0) {
    swap_orders(first, mid, elem_size);
  }
  swap_orders(at(array, start + 1, elem_size), mid, elem_size);
}

static int getPartitionIndex(void* array, int start, int end, size_t elem_size, compare_fn cmp) {
  //after calling the mediansOfThree, we've already sorted first two elements and the last element
  mediansOfThree(array, start, end, elem_size, cmp);
  void* pivot = at(array, start + 1, elem_size);
  // int pivotIdx = start + 1;
  int leftIdx = start + 2;
  int rightIdx = end - 1;
  while(leftIdx < rightIdx) {
    while(cmp(at(array, leftIdx, elem_size), pivot) >= 0) leftIdx++;
    while(cmp(at(array, rightIdx, elem_size), pivot) < 0) rightIdx--;
    if (leftIdx < rightIdx) swap_orders(at(array, leftIdx, elem_size), at(array, rightIdx, elem_size), elem_size);
  }
  
  swap_orders(at(array, rightIdx, elem_size), pivot, elem_size);
  return rightIdx;
}

void quickSort(int start, int end, void *array, size_t elem_size, compare_fn cmp) {
  if (start >= end) return;
  if(end - start + 1 < INSERTION_SORT_THRESHOLD) {
    insertionSort(array, start, end, elem_size, cmp);
    return;
  }
  int pivotIdx = getPartitionIndex(array, start, end, elem_size, cmp);
  quickSort(0, pivotIdx - 1, array, elem_size, cmp);
  quickSort(pivotIdx + 1, end, array, elem_size, cmp);
}
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Merge Sort: 
/////////////////////////////////////////////////////////////////////////////////
static void merge(int start, int middle, int end, void* array, size_t elem_size, compare_fn cmp) {
  //left arr[left, mid] right arr[mid + 1, end]
  int left_size = middle - start + 1;
  int right_size = end - middle;
  
  void* lt = malloc(elem_size * left_size);
  void* rt = malloc(elem_size * right_size);
  void* tmp = malloc(elem_size * (end - start + 1));
 
  for(int i = 0; i < left_size; ++i) {
    memcpy(at(lt, i, elem_size), at(array, start + i, elem_size), elem_size);
  }
  for(int j = 0; j < right_size; ++j) {
    memcpy(at(rt, j, elem_size), at(array, middle + 1 + j, elem_size), elem_size);
  }

  int left, right, c_start;
  left = 0, right = 0, c_start = 0;
  while(left < left_size && right < right_size) {
    if(cmp(at(lt, left, elem_size), at(rt, right, elem_size)) < 0) {
      memcpy(at(tmp, c_start, elem_size), at(rt, right, elem_size), elem_size);
      ++right;
    } else {
      memcpy(at(tmp, c_start, elem_size), at(lt, left, elem_size), elem_size);
      ++left;
    }
    ++c_start;
  }

  while(left < left_size) {
    memcpy(at(tmp, c_start, elem_size), at(lt, left, elem_size), elem_size);
    ++left;
    ++c_start;
  }

  while(right < right_size) {
    memcpy(at(tmp, c_start, elem_size), at(rt, right, elem_size), elem_size);
    ++right;
    ++c_start;
  }

  memcpy(at(array, start, elem_size), tmp, elem_size * (end - start + 1));
  free(lt);
  free(rt);
  free(tmp);
}

void mergeSort(int start, int end, void* array, size_t elem_size, compare_fn cmp) {
  if(start < end) {
    int mid = start + (end - start) / 2;
    mergeSort(start, mid, array, elem_size, cmp);
    mergeSort(mid+ 1, end, array, elem_size, cmp);

    merge(start, mid, end, array, elem_size, cmp);
  }
}
#ifndef CASHES__ARRAY_GENERATION_H_
#define CASHES__ARRAY_GENERATION_H_

#include <stdlib.h>

typedef enum {
  LINKED_FORWARD,
  LINKED_BACKWARDS,
  LINKED_RANDOM
} WalkOrder;

#define FILL_INDEX 0

#define walkOrderName(order) ((order) == LINKED_RANDOM ? "LINKED_RANDOM" : (order == LINKED_BACKWARDS ? "LINKED_BACKWARDS" : "LINKED_FORWARD"))

unsigned int *createArray(size_t size, int fill_offset, int fill_zeroes) {

  unsigned int *arr_ptr = (unsigned int *) calloc(size, sizeof(unsigned int));

  if (arr_ptr == NULL) {
    return NULL;
  }

  if (!fill_zeroes) {
    while (fill_offset < 0) {
      fill_offset += size;
    }

    for (size_t i = 0; i < size; ++i) {
      arr_ptr[i] = (fill_offset + i) % size;
    }
  }

  return arr_ptr;
}

int randomCompare(void const *a, void const *b) {
  return rand() - RAND_MAX / 2;
}

unsigned int *newPermutation(size_t n) {

  unsigned int *new_permutation = createArray(n, 0, FILL_INDEX);

  if (new_permutation == NULL) {
    return NULL;
  }

  qsort(new_permutation, n, sizeof(unsigned int), randomCompare);

  return new_permutation;
}

unsigned int *createLinkedArray(size_t size,WalkOrder order) {

  if (size == 0) {
    return NULL;
  }

  int offset = order == LINKED_RANDOM ? 0 : order == LINKED_BACKWARDS ? -1 : 1;

  unsigned int *arr_ptr = createArray(size, offset, order == LINKED_RANDOM);

  if (arr_ptr == NULL) {
    return NULL;
  }

  if (order == LINKED_RANDOM) {
    unsigned int *permutation = newPermutation(size);

    for (size_t i = 0; i < size - 1; ++i) {
      arr_ptr[permutation[i]] = permutation[i + 1];
    }

    arr_ptr[permutation[size - 1]] = permutation[0];

    free(permutation);
  }

  return arr_ptr;
}

#endif //CASHES__ARRAY_GENERATION_H_

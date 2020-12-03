#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "array_generation.h"

#define NUM 35

typedef uint64_t time_result[NUM];

#define MIN(a, b, c) ( ((a) < (b) ? (a) : (b)) < (c) ? ((a) < (b) ? (a) : (b)) : (c))

void printResult(time_result result) {
 size_t arr_size = 1024;
 for (size_t i = 0; i < 12; ++i) {
   printf("%luK\t\t\t%lu\n", arr_size / 1024, result[i]);
   arr_size *= 2;
 }
 for (size_t i = 12; i < NUM; ++i) {
   printf("%luK\t\t\t%lu\n", arr_size / 1024, result[i]);
   arr_size += 131072;//после 4мб шаг 128КБ
 }

 printf("\n");
}

static inline uint64_t rdtsc(void) {
 unsigned hi, lo;
 asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
 return ((uint64_t) lo) | (((uint64_t) hi) << 32u);
}

static size_t prog(size_t size){
 return (size-1)*size/2;

}

static uint64_t walkEveryElement(size_t n_walks, size_t array_size_bytes, WalkOrder order) {

 size_t array_size = array_size_bytes / sizeof(unsigned int);
 register unsigned int *a = NULL;

 L1:
 a = createLinkedArray(array_size, order);

 long long int sum = 0;

 for (size_t k = 0, i = 0; i < array_size; ++i) {//прогрев кэша
   k = a[k];
   sum +=k;
 }

 if(sum !=prog(array_size)){// проверяем все ли мы обошили, если нет то генерируем новый массив
   free(a);
   goto L1;
 }

 uint64_t s, e;

 s = rdtsc();

 for (register size_t k = 0, i = 0; i < n_walks * array_size; ++i) {
   k = a[k];
 }

 free(a);

 e = rdtsc();

 uint64_t d = e - s;

 return d / (n_walks * array_size);
}

void doAllWalks() {
 time_result result[3];
 time_result final_result;

 WalkOrder order[3] = {LINKED_BACKWARDS, LINKED_FORWARD, LINKED_RANDOM};

 for (size_t j = 0; j < 3; ++j) {

   for (size_t k = 0; k < 3; ++k) {
     size_t arr_size = 1024;
     register size_t i = 0;

     for (; i < 12; ++i) {
       result[k][i] = walkEveryElement(6, arr_size, order[j]);
       arr_size *= 2;
     }

     for (; i < NUM; ++i) {
       result[k][i] = walkEveryElement(6, arr_size, order[j]);
       arr_size += 131072;
     }

   }

   printf("%s\n", walkOrderName(order[j]));

   for (size_t k = 0; k < NUM; ++k) {
     final_result[k] = MIN(result[0][k], result[1][k], result[2][k]);
   }

   printResult(final_result);
 }
}

int main() {
 srand(time(NULL));

 doAllWalks();

 return 0;
}

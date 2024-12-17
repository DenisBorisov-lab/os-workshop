#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

static const int ARRAY_SIZE = 10000000;

typedef struct {
  int *array;            // указатель на массив
  int low;               // нижняя граница массива
  int high;              // верхняя граница
  int is_heap_allocated; // флаг выделения
} SortParams;

int max_threads;
int current_threads = 0;
pthread_mutex_t thread_count_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *thread_semaphore;

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

int partition(int arr[], int low, int high) {
  int pivot = arr[high];
  int i = (low - 1);

  for (int j = low; j <= high - 1; j++) {
    if (arr[j] < pivot) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}

void *quicksort(void *arg) {

  SortParams *params = (SortParams *) arg;

  int low = params->low;
  int high = params->high;
  int *arr = params->array;
  int is_heap_allocated = params->is_heap_allocated;

  if (low < high) {
    int pivot_index = partition(arr, low, high);

    pthread_t thread_left, thread_right;
    int create_new_threads = 0;

    pthread_mutex_lock(&thread_count_mutex);
    if (current_threads < max_threads - 1) {
      current_threads += 2;
      create_new_threads = 1;
    }
    pthread_mutex_unlock(&thread_count_mutex);

    if (create_new_threads) {
      sem_wait(thread_semaphore);
      sem_wait(thread_semaphore);

      SortParams *left_params = malloc(sizeof(SortParams));
      SortParams *right_params = malloc(sizeof(SortParams));

      left_params->array = arr;
      left_params->low = low;
      left_params->high = pivot_index - 1;
      left_params->is_heap_allocated = 1;

      right_params->array = arr;
      right_params->low = pivot_index + 1;
      right_params->high = high;
      right_params->is_heap_allocated = 1;

      pthread_create(&thread_left, NULL, quicksort, left_params);
      pthread_create(&thread_right, NULL, quicksort, right_params);

      pthread_join(thread_left, NULL);
      pthread_join(thread_right, NULL);

      sem_post(thread_semaphore);
      sem_post(thread_semaphore);

      pthread_mutex_lock(&thread_count_mutex);
      current_threads -= 2;
      pthread_mutex_unlock(&thread_count_mutex);
    } else {
      SortParams left_params = {arr, low, pivot_index - 1, 0};
      SortParams right_params = {arr, pivot_index + 1, high, 0};
      quicksort(&left_params);
      quicksort(&right_params);
    }
  }

  if (is_heap_allocated) {
    free(params);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Unsupported arguments length! Usage: %s <max_threads>\n", argv[0]);
    return 1;
  }

  max_threads = atoi(argv[1]);

  if (max_threads < 1) {
    printf("Max threads must be at least 1\n");
    return 1;
  }

  const char *semaphore_name = "/quicksort_semaphore";
  sem_unlink(semaphore_name);
  thread_semaphore = sem_open(semaphore_name, O_CREAT | O_EXCL, 0644, max_threads);
  if (thread_semaphore == SEM_FAILED) {
    perror("sem_open failed");
    return 1;
  }

  int *array = malloc(ARRAY_SIZE * sizeof(int));
  for (int i = 0; i < ARRAY_SIZE; i++) {
    array[i] = rand() % 1000000 - 5000;
  }

  SortParams initial_params = {
      .array = array,
      .low = 0,
      .high = ARRAY_SIZE - 1,
      .is_heap_allocated = 0
  };

  printf("Array before sorting:\n");
  for (int i = 0; i < ARRAY_SIZE; i++) {
//    printf("%d ", array[i]);
  }
  printf("\n");

  printf("Sorting array of %d elements with max %d threads...\n", ARRAY_SIZE, max_threads);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  quicksort(&initial_params);

  clock_gettime(CLOCK_MONOTONIC, &end);

  double time_spent = (end.tv_sec - start.tv_sec) +
      (end.tv_nsec - start.tv_nsec) / 1000000000.0;

  printf("Sorting took %f seconds\n", time_spent);

  printf("Array after sorting:\n");
  for (int i = 0; i < ARRAY_SIZE; i++) {
//    printf("%d ", array[i]);
  }
  printf("\n");

  free(array);
  sem_close(thread_semaphore);
  sem_unlink(semaphore_name);
  pthread_mutex_destroy(&thread_count_mutex);

  return 0;
}

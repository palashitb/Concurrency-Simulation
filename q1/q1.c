#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <math.h>

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}

int partition (int *arr, int low, int high){
     int pivot = arr[high]; // pivot
     int i=low-1;
     for (int j=low; j<=high-1; j++){
          if (arr[j] < pivot){
               i++;
               swap(&arr[i], &arr[j]);
          }
     }
     swap(&arr[i + 1], &arr[high]);
     return (i + 1);
}

void normal_quickSort(int *arr, int low, int high){
     if(low<high){
          /* pi is partitioning index, arr[p] is now
          at right place */
          int pi = partition(arr, low, high);

          // Separately sort elements before
          // partition and after partition
          normal_quickSort(arr, low, pi - 1);
          normal_quickSort(arr, pi + 1, high);
     }
}

void merge(int *arr, int low, int mid, int high){
     int temp[high - low + 1];
     int l = 0, h = 0;
     while( l < mid - low + 1 && h < high - mid ){
          if( arr[l + low] < arr[mid + h + 1] ){
               temp[l+h] = arr[l + low];
               l++;
          }
          else{
               temp[l + h] = arr[mid + h + 1];
               h++;
          }
          
     }
     while( l < mid - low + 1 ){
               temp[l + h] = arr[l + low];
               l++;
     }
     while( h < high - mid ){
               temp[l + h] = arr[mid + h + 1];
               h++;
     }
     for(int i = 0 ; i <= high - low ; i++)
          arr[low + i] = temp[i];
}

void mergeSort(int *arr, int low, int high){
     if( high - low <= 4 )
          normal_quickSort(arr, low, high);
     else{
         int pid1 = fork();
         int pid2;
         int mid = low + (high - low) / 2;
         if( pid1 == 0 ){
              mergeSort(arr, low, mid);
              _exit(1);
         } 
         pid2 = fork();
         if( pid2 == 0 ){
              mergeSort(arr, mid + 1, high);
              _exit(1);
         }
         int status;
         waitpid(pid1, &status, 0);
         waitpid(pid2, &status, 0);
         merge(arr, low, mid, high);
     }
}

struct arg{
     int l;
     int r;
     int* arr;
};

void *threaded_mergeSort(void* a){
     //note that we are passing a struct to the threads for simplicity.
     struct arg *args = (struct arg*) a;

     int l = args->l;
     int r = args->r;
     int *arr = args->arr;
     if(l>=r) return NULL;


     int mid = l + (r - l) / 2;
     //sort left half array
     struct arg a1;
     a1.l = l;
     a1.r = mid;
     a1.arr = arr;
     pthread_t tid1;
     pthread_create(&tid1, NULL, threaded_mergeSort, &a1);

     //sort right half array
     struct arg a2;
     a2.l = mid+1;
     a2.r = r;
     a2.arr = arr;
     pthread_t tid2;
     pthread_create(&tid2, NULL, threaded_mergeSort, &a2);

     //wait for the two halves to get sorted
     pthread_join(tid1, NULL);
     pthread_join(tid2, NULL);
     merge(arr, l, mid, r);

     //merge(arr,l,r);
}

void normal_mergeSort(int *arr, int low, int high){
     if( high > low ){
          int mid = low + (high - low) / 2;
          normal_mergeSort(arr, low, mid);
          normal_mergeSort(arr, mid + 1, high);
          merge(arr, low, mid, high);
     }
}

void runSorts(long long int n){

     struct timespec ts;

     //getting shared memory
     int *arr = shareMem(sizeof(int)*(n+1));
     for(int i=0;i<n;i++) scanf("%d", arr+i);
     int crr[n];
     for(int i = 0 ; i < n ; i++)
          crr[i] = arr[i];

     int brr[n+1];
     for(int i=0;i<n;i++) brr[i] = arr[i];

     printf("Running concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multiprocess mergesort
     mergeSort(arr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",arr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t1 = en-st;

     pthread_t tid;
     struct arg a;
     a.l = 0;
     a.r = n-1;
     a.arr = arr;
     printf("Running threaded_concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multithreaded mergesort
     pthread_create(&tid, NULL, threaded_mergeSort, &a);
     pthread_join(tid, NULL);
     for(int i=0; i<n; i++){
          printf("%d ",a.arr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t2 = en-st;

     printf("Running normal_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     // normal mergesort
     normal_mergeSort(brr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",brr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t3 = en - st;

     printf("normal_mergesort ran:\n\t[ %Lf ] times faster than concurrent_mergesort\n\t[ %Lf ] times faster than threaded_concurrent_mergesort\n\n\n", t1/t3, t2/t3);
     shmdt(arr);
     return;
}

int main(){

     long long int n;
     scanf("%lld", &n);
     runSorts(n);
     return 0;
}

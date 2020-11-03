#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

long long int min(long long int a, long long int b){
    if( a > b )
        return b;
    return a;
}

int cur, vax_left[100], m, n, o, stud_start, stud_end;
pthread_t threads[1000];
struct universal {
    int batches_remaining, supplier, batch_size, duration, index, batches, alloted, round;
    float prob;
};
struct universal info[1000];

void* zones_procure(void *a){
    struct universal *s = a;
    while( s -> batch_size > 0 ){
        int this_batch = 1 + min(7, rand() % s -> batch_size);
        s -> batch_size -= this_batch;
        int students[this_batch];
        for(int i = stud_start ; i < stud_end && this_batch > 0 ; i++){
            if( info[i].alloted == 0 and info[i].round < 3 ){
                info[i].alloted = 1;
                info[i].round++;
                this_batch--;
            }
        }
    }
}

void* manufacture(void *a){
    struct universal *company = a;
    company -> batches_remaining = 1 + rand() % 5;
    company -> batches = company -> batches_remaining;
    company -> batch_size = 10 + rand() % 11;
    company -> duration = 2 + rand() % 4;
    for(int i = 0 ; i < m ; i++){
        if( vax_left[i] == 0 ){
            vax_left[i] = company -> batch_size;
            info[i].supplier = company -> index;
            pthread_create(&threads[cur], NULL, zones_procure, &info[i]);
        }
        company -> batches--;
        if( company -> batches == 0 )
            break;
    }
}

int main(){
    cur = 0;
    scanf("%d %d %d", &n, &m, &o);
    for(int i = 0 ; i < m ; i++){
        info[i].index = i;
        vax_left[i] = 0;
        cur++;
    }
    for(int i = 0 ; i < n ; i++){
        float prob;
        scanf("%f", &prob);
        info[cur].index = cur;
        info[cur].prob = prob;
        pthread_create(&threads[cur], NULL, manufacture, &info[cur]);
        cur++;
    }
    stud_start = cur;
    for(int i = 0 ; i < o ; i++){
        info[cur].duration = rand() % 20;
        info[cur].index = cur;
        pthread_create(&threads[cur], NULL, kids, &info[cur]);
        cur++;
    }
    stud_end = cur;
    for(int i = 0 ; i < cur ; i++){
        pthread_join(threads[i], NULL);
    }
}
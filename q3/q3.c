#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <errno.h>

//coloring
void red(){
    printf("\033[0;31m");
}

void yellow(){
    printf("\033[0;33m");
}

void green(){
    printf("\033[0;32m");
}

void blue(){
    printf("\033[0;34m");
}

void magenta(){
    printf("\033[0;35m");
}

void cyan(){
    printf("\033[0;36m");
}

void reset(){
    printf("\033[0m");
}

int k, aa, e, c, t, t1, t2;
int e_end = 0, a_end = 0, cur = 0, maxi = 0, scur = 0, smax = 0;
int stage_taken[100], singer_done[100], stage_over[100];
sem_t acoustic, electric, usr, awards;

struct singer{
	char *name;
	int arrival, used, i, duration;
	pthread_t thread;
};

struct arg{
	int i, executed, taken, duration, finished;
	int *tim, *end;
	char *name, *inst;
	struct singer *s;
};

struct arg stage[100];

struct arg *stgcpy(struct arg *a, struct arg *b){
	a -> i = b -> i;
	a -> finished = b -> finished;
	a -> executed = b -> executed;
	a -> taken = b -> taken;
	a -> duration = b -> duration;
	a -> tim = b -> tim;
	a -> end = b -> end;
	a -> name = b -> name;
	a -> inst = b -> inst;
	a -> s = b -> s;
}

struct singer *singcpy(struct singer *a, struct singer *b){
	a -> name = b -> name;
	a -> i = b -> i;
	a -> arrival = b -> arrival;
	a -> used = b -> used;
	a -> duration = b -> duration;
	a -> thread = b -> thread;
}

long long int min(long long int a, long long int b){
	if( a > b )
		return b;
	return a;
}

void* esolo(void *a){
	yellow();
	struct singer s;
	sem_wait(&electric);
	sem_wait(&usr);
	singcpy(&s, a);
	if( singer_done[s.i] == 1 ){
		sem_post(&electric);
		sem_post(&usr);
		return NULL;
	}
	singer_done[s.i] = 1;
	sem_post(&usr);
	printf("%s singer enters the electric stage for solo\n", s.name);
	sleep(s.duration);
	cyan();
	printf("%s singer completed their performance at the electric stage\n", s.name);
	sem_post(&electric);
}

void* asolo(void*a){
	magenta();
	struct singer s;
	sem_wait(&acoustic);
	sem_wait(&usr);
	singcpy(&s, a);
	if( singer_done[s.i] = 1 ){
		sem_post(&acoustic);
		sem_post(&usr);
		return NULL;
	}
	singer_done[s.i] = 1;
	red();
	sem_post(&usr);
	printf("%s singer enters the acoustic stage for solo\n", s.name);
	sleep(s.duration);
	cyan();
	printf("%s singer completed their performance at the acoustic stage\n", s.name);
	sem_post(&acoustic);
}

void* join(void *a){
	green();
	struct singer s;
	sem_wait(&usr);
	singcpy(&s, a);
	while( cur < maxi && (stage_taken[cur] == 1 || stage_over[cur] == 1 ) )
		cur++;
	if( singer_done[s.i] == 1 ){
		sem_post(&usr);
		return NULL;
	}
	if( cur < maxi ){
		blue();
		singer_done[s.i] = 1;
		printf("Singer %s has joined %s in the ", s.name, stage[cur].name);
		stage[cur].s = &s;
		stage_taken[cur] = 1;
		if( stage[cur].executed == 1 )
			printf("electric stage\n");
		else
			printf("acoustic stage\n");
		cur++;
	}
	sem_post(&usr);
}

void* singre(void *s){
	cyan();
	struct singer si;
	sem_wait(&usr);
	singcpy(&si, s);
	sem_post(&usr);
	sleep(si.arrival);
	sem_wait(&usr);
	singer_done[smax] = 0;
	si.i = smax;
	smax++;
	sem_post(&usr);
	printf("%s (singer) arrives at Srujana\n", si.name);
	pthread_t ag, bg, cg;
	pthread_create(&ag, NULL, esolo, &si);
	pthread_create(&bg, NULL, asolo, &si);
	pthread_create(&cg, NULL, join, &si);
	sleep(t);
	pthread_join(ag, NULL);
	pthread_join(bg, NULL);
	pthread_join(cg, NULL);
	sem_wait(&usr);
	green();
	if( singer_done[si.i] == 0 )
		printf("%s left the event in anger\n", si.name);
	sem_post(&usr);
	pthread_join(si.thread, NULL);
}

void *award(void *a){
	red();
	sem_wait(&usr);
	struct arg ar;
	stgcpy(&ar, a);
	sem_post(&usr);
	yellow();
	printf("%s is waiting for t-shirt\n", ar.name);
	sem_wait(&awards);
	green();
	printf("%s is collecting their t-shirt\n", ar.name);
	sleep(2);
	red();
	printf("%s has exited\n", ar.name);
	sem_post(&awards);
}

void* estage(void *aa){
    struct arg *ar = (struct arg*) aa;
	sleep(ar -> tim[ar -> i]);
	cyan();
	printf("%s arrived at Srujana\n", ar -> name);
	struct timespec ts;
	ts.tv_sec += t;
	int ss;
	while( ( ss = sem_timedwait(&electric, &ts) ) == -1 && errno == EINTR )
		continue;
	if( ss == -1 ){
		printf("%s exited the event with anger\n", ar -> name);
		return NULL;
	}
	sem_wait(&usr);
	if( ar -> executed == 0 )
		ar -> executed = 1;
	else{
		sem_post(&electric);
		sem_post(&usr);
		return NULL;
	}
	stgcpy(&stage[maxi], ar);
	int you = maxi;
	stage_taken[you] = 0;
	stage_over[you] = 0;
	maxi++;
	sem_post(&usr);
	e_end += ar -> duration;
	ar -> end[ar -> i] = e_end;	
	red();
	printf("%s is playing at the Electric stage with instrument %c for a duration of %d\n", ar -> name, ar -> inst[ar -> i], ar -> duration);
	sleep(ar -> duration);
	sem_wait(&usr);
	blue();
	if( stage_taken[you] == 1 ){
		sem_post(&usr);
		sleep(2);
		printf("%s and singer ended their performance at electric stage\nThe singer exited\n", ar -> name);
		sem_wait(&usr);
		stage_over[you] = 1;
		sem_post(&usr);
		sem_post(&electric);
	}
	else{
		printf("%s ended their performance at electric stage\n", ar -> name);
		stage_over[you] = 1;
		sem_post(&usr);
		sem_post(&electric);
	}
	pthread_t ttt;
	pthread_create(&ttt, NULL, award, ar);
	pthread_join(ttt, NULL);
}

void* astage(void *aa){
	magenta();
    struct arg *ar = (struct arg*) aa;
	sleep(ar -> tim[ar -> i]);
	if( ar -> inst[ar -> i] == 'b' ){
		green();
		printf("%s arrived at Srujana\n", ar -> name);
	}
	// sem_wait(&acoustic);
	struct timespec ts;
	ts.tv_sec += t;
	int ss;
	while( ( ss = sem_timedwait(&acoustic, &ts) ) == -1 && errno == EINTR )
		continue;
	if( ss == -1 && ar -> inst[ar -> i] == 'b' ){
		yellow();
		printf("%s exited the event with anger\n", ar -> name);
		return NULL;
	}
	else if( ss == -1 )
		return NULL;
	sem_wait(&usr);
	if( ar -> executed == 0 )
		ar -> executed = 2;
	else{
		sem_post(&acoustic);
		sem_post(&usr);
		return NULL;
	}
	stgcpy(&stage[maxi], ar);
	int you = maxi;
	stage_taken[you] = 0;
	stage_over[you] = 0;
	maxi++;
	sem_post(&usr);
	a_end += ar -> duration;
	ar -> end[ar -> i] = a_end;
	red();
	printf("%s is playing at the acoustic stage with instrument %c for a duration of %d\n", ar -> name, ar -> inst[ar -> i], ar -> duration);
	sleep(ar -> duration);
	sem_wait(&usr);
	blue();
	if( stage_taken[you] == 1 ){
		sem_post(&usr);
		sleep(2);
		printf("%s and singer ended their performance at acoustic stage\nThe singer exited\n", ar -> name);
		sem_wait(&usr);
	}
	else
		printf("%s ended their performance at acoustic stage\n", ar -> name);
	stage_over[you] = 1;
	sem_post(&usr);
	sem_post(&acoustic);
	pthread_t ttt;
	pthread_create(&ttt, NULL, award, ar);
	pthread_join(ttt, NULL);
}

void sortasort(pthread_t a[][2], int *end, int k){
	for(int i = 0 ; i < k ; i++){
		int mini = i;
		for(int j = i ; j < k ; j++)
			if( end[j] < end[mini] )
				mini = j;
		
		long int temp1 = end[i];
		long int temp2 = a[i][0];
		end[i] = end[mini];
		a[i][0] = a[mini][0];
		a[i][1] = end[i];
		end[mini] = temp1;
		a[mini][0] = temp2;
	}
}

int main(){
	sem_init(&usr, 0, 1);
	pthread_t a[100][2];
	char inp[100][100], instrument[100];
	int end[100];
	int tyme[100];
	time_t tt;
	struct arg info[100];
	srand((unsigned) time(&tt));
	scanf("%d %d %d %d %d %d %d", &k, &aa, &e, &c, &t1, &t2, &t);
	sem_init(&electric, 0, e);
	sem_init(&acoustic, 0, aa);
	sem_init(&awards, 0, c);
	for(int i = 0 ; i < k ; i++){
		int ttime;
		char tname[100];
		char tinst;
		scanf("%s %c %d", tname, &tinst, &ttime);
		if( tinst == 's' ){
			pthread_t sin;
			struct singer s;
			s.used = 0;
			s.name = tname;
			s.duration = t1 + rand() % (t2 - t1 + 1);
			s.arrival = ttime;
			pthread_create(&s.thread, NULL, singre, &s);
			continue;
		}
		strcpy(inp[i], tname);
		instrument[i] = tinst;
		tyme[i] = ttime;
		info[i].name = inp[i];
		info[i].inst = instrument;
		info[i].tim = tyme;
		info[i].i = i;
		info[i].executed = 0;
		info[i].finished = 0;
		info[i].taken = 0;
		info[i].end = end;
		info[i].s = NULL;
		info[i].duration = t1 + rand() % (t2 - t1 + 1);
		a[i][0] = 0;
		a[i][1] = 0;
		if( instrument[i] != 'b' ){
			end[i] = min(end[i], a_end);
			pthread_create(&a[i][0], NULL, estage, &info[i]);
		}
		if( instrument[i] != 'v' ){
			end[i] = min(end[i], e_end);
			pthread_create(&a[i][1], NULL, astage, &info[i]);
		}
	}
	for(int i = 0 ; i < k ; i++)
		if( info[i].executed == 2 )
			a[i][0] = a[i][1];
	
	sortasort(a, end, k);
	for(int i = 0 ; i < k ; i++)
		pthread_join(a[i][0], NULL);
	red();
	printf("Finished\n");
	return 0;
}
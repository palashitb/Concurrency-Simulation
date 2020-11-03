#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
int n, m, o, oo, cur, zone1, tame, zone2;
pthread_mutex_t shm;

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

int min(int a, int b){
	if( a > b )
		return b;
	return a;
}

struct universal {
	int index, type, size, tt, round, slots, person, supplier, remaining, duration, feedback;
	int kids[100];
	float prob;
};
struct universal info[1000];
pthread_t threads[1000];

void* kids(void *a){
	int flag = 0;
	struct universal *s = a;
	sleep(s -> duration);
	yellow();
	printf("Student %d has arrived for Round %d of vaccination\n", s -> person, s -> round);
	sleep(tame);
	while( flag == 0 ){
		for(int i = zone1 ; i < zone2 ; i++)
			if( info[i].remaining > 0 ){
				pthread_mutex_lock(&shm);
				if( info[i].remaining == 0 )
					continue;
				info[i].remaining = info[i].remaining - 1;
				flag = 1;
				cyan();
				printf("Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n", s -> person, info[i].person);
				sleep(tame);
				oo--;
				// printf("oo is %d and o is %d\n", oo, o);
				info[i].kids[info[i].tt] = s -> index ;
				info[i].tt = info[i].tt + 1;
				pthread_mutex_unlock(&shm);
				break;
			}
	}
	return NULL;
}

void* zone(void *a){
	struct universal *zone = a;
	while( o > 0 ){
		while(zone -> size == 0 && o > 0 );
		sleep(tame);
		red();
		printf("Zone %d has been given %d sized batch\n", zone -> person, zone -> size);
		while( zone -> size > 0 && o > 0 ){
			pthread_mutex_lock(&shm);
			zone -> remaining = 1 + min(7, rand() % zone -> size);
			zone -> slots = zone -> remaining;
			sleep(tame);
			green();
			printf("Vaccination zone %d is ready to vaccinate with %d slots\n", zone -> person, zone -> remaining);
			pthread_mutex_unlock(&shm);
			while( zone -> remaining > 0 && oo > 0 && o > 0 );
			sleep(tame);
			blue();
			if( zone -> remaining < info[zone -> supplier].size )
				printf("Vaccination zone %d is beginning to vaccinate\n", zone -> person);
			pthread_mutex_lock(&shm);
			for(int i = 0 ; i < zone -> tt && o > 0 ; i++){
				sleep(tame);
				cyan();
				printf("Student %d on Vaccination Zone Y has been vaccinated which has success probability %d Where %f is the probability ​of that vaccine successfully administrating antibodies\n", zone -> kids[i] + 1, zone -> person, zone -> prob);
				if( rand() % 100 >= (int)(zone -> prob * 100) ){
					info[zone -> kids[i]].round = info[zone -> kids[i]].round + 1;
					sleep(tame);
					magenta();
					printf("Student %d failed the antibodies test\n", zone -> kids[i] + 1);
					if( info[zone -> kids[i]].round > 3 ){
						sleep(tame);
						green();
						printf("Student %d has been rejected\n", zone -> kids[i] + 1);
						continue;
					}
					info[zone -> kids[i]].duration = 0;
					oo++;
					pthread_create(&threads[cur], NULL, kids, &info[zone -> kids[i]]);
					cur++;
				}
				else{
					o--;
					sleep(tame);
					magenta();
					printf("Student %d has passed the antibodies test and moving into campus\n", zone -> kids[i] + 1);
				}
			}
			zone -> size = zone -> size + zone -> remaining - zone -> slots;
			pthread_mutex_unlock(&shm);
		}
		if( zone -> size == 0 ){
			pthread_mutex_lock(&shm);
			info[(zone -> supplier)].feedback--;
			pthread_mutex_unlock(&shm);
			sleep(tame);
			green();
			printf("Zone %d has run out of vaccines :(\n", zone -> person);
		}
	}
	return NULL;
}

void* company(void *a){
	struct universal *s = a;
	while( o > 0 ){
		while( s -> remaining > 0 && o > 0 );
		pthread_mutex_lock(&shm);
		s -> duration = 2 + rand() % 4;
		s -> size = 10 + rand() % 11;
		s -> remaining = 1 + rand() % 5;
		pthread_mutex_unlock(&shm);
		sleep(tame);
		yellow();
		printf("Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %f\n", s -> person, s -> remaining, s -> prob);
		sleep(s -> duration);
		blue();
		printf("Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %f. Waiting for all the vaccines to be used to resume production\n", s -> person, s -> remaining, s -> prob);
		pthread_mutex_lock(&shm);
		for(int i = zone1 ; i < zone2 ; i++){
			if( info[i].size == 0 ){
				info[i].size = s -> size;
				info[i].prob = s -> prob;
				s -> feedback = s -> feedback + 1;
				info[i].supplier = s -> person;
				sleep(tame);
				cyan();
				printf("Pharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %f\n", s -> person, info[i].person, s -> prob);
				s -> remaining = s -> remaining - 1;
				if( s -> remaining == 0 )
					break;
			}
		}
		pthread_mutex_unlock(&shm);
		while( (s -> feedback > 0 || s -> remaining > 0) && o > 0 );
		sleep(tame);
		yellow();
		if( s -> size == 0 )
			printf("All the vaccines prepared by Pharmaceutical Company %d are emptied.\n", s -> person);
		pthread_mutex_lock(&shm);
		s -> size = 0;
		pthread_mutex_unlock(&shm);
	}
	return NULL;
}

int main(){
	tame = 0;     //set the value of tame as per your requirements(high tame for slow simulation).
	pthread_mutex_init(&shm, NULL);
	srand(time(0));
	scanf("%d %d %d", &n, &m, &o);
	oo = o;
	pthread_mutex_lock(&shm);
	for(int i = 0 ; i < o ; i++){
		info[cur].duration = 1 + rand() % 7;
		info[cur].index = cur;
		info[cur].round = 1;
		info[cur].person = i + 1;
		pthread_create(&threads[cur], NULL, kids, &info[cur]);
		cur++;
	}
	zone1 = cur;
	zone2 = zone1 + m - 1;
	for(int i = zone1 ; i < zone2 ; i++){
		info[cur].index = cur;
		info[cur].size = 0;
		info[cur].duration = 0;
		info[cur].person = i - zone1 + 1;
		info[cur].remaining = 0;
		pthread_create(&threads[cur], NULL, zone, &info[cur]);
		cur++;
	}
	for(int i = cur ; i < n + zone2 ; i++){
		float prob;
		scanf("%f", &prob);
		info[cur].person = i - zone2 + 1;
		info[cur].index = cur;
		info[cur].size = 0;
		info[cur].feedback = 0;
		info[cur].remaining = 0;
		info[cur].prob = prob;
		pthread_create(&threads[cur], NULL, company, &info[cur]);
		cur++;
	}
	pthread_mutex_unlock(&shm);
	for(int i = 0 ; i < cur ; i++){
		pthread_join(threads[i], NULL);
	}
	sleep(tame);
	printf("Simulation over. :P\n");
	reset();
	return 0;
}
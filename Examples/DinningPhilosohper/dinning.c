#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUMDINERS   5
#define EAT_TIMES   3


#define LEFT(philID)    (philID)
#define RIGHT(philID)   (((philID)+1)%NUMDINERS)

/* marcos to conveniently refer to forks to left and right of each person */

sem_t forks[NUMDINERS]; // all  forks available 
sem_t numAllowedToEat;  // allowed to eat == NUMDINERS -1 

int threadid[NUMDINERS]={0};

static void Thinking(char buf[])
{
    
       printf("%s thinking!\n", buf); 
       usleep(100000*(rand()%5));
}

static void Eating(char name[], int id)
{
       sem_wait(&numAllowedToEat);
       sem_wait(&forks[LEFT(id)]);
       sem_wait(&forks[RIGHT(id)]);
       printf("%s eating!\n", name);
       usleep(50000*(rand()%5));
       sem_post(&forks[LEFT(id)]);
       sem_post(&forks[RIGHT(id)]);
       sem_post(&numAllowedToEat);
}

void* philosopher(void* arg)
{
    int id = *(int*)arg;
    char name[32] ={0};
    sprintf(name,"Philosopher %d", id);
    for(int i=0; i< EAT_TIMES; ++i)
    {
       // thinking  
       Thinking(name);
       // eating
       Eating(name, id);
    }
    printf("%s done and gone asleep!\n", name);
    return arg;
}

int main()
{
    srand((unsigned int )time(NULL));
    // semaphore initialization 信号量初始化
    for(int i=0; i<NUMDINERS;++i)
    {
        sem_init(&forks[i], 0, 1); // all set to 1
    }
    
    sem_init(&numAllowedToEat, 0, NUMDINERS-1); // NUMDINERS -1 phils allows to eat, thus at least one can grab 2 forks and eat.
    
    pthread_t tid[NUMDINERS];

    // create NUMDINERS threads and do their life works.
    for(int i=0; i<NUMDINERS; ++i)
    {
        threadid[i]=i;
        pthread_create(&tid[i], NULL, philosopher, &threadid[i]);
        // pthread_detach(tid[i]);
    }

    
    for(int i=0; i< NUMDINERS; ++i)
    {
        pthread_join(tid[i], NULL);
    }


    sem_destroy(&numAllowedToEat);
    for(int i=0 ;i<NUMDINERS; ++i)
    {
        sem_destroy(&forks[i]);
    }
    printf("all done!\n");
    return 0;
}


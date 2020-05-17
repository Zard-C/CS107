#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#define NUMTICKAGENT 16
#define TICKETSTOSELL 150;

/*
 *  global shared resources: numTickets, num of tickets to sell 
 *                           lock, mutex_t to protect numTickets 
 */

int numTickets = TICKETSTOSELL;
pthread_mutex_t lock;

/*
 *  information passed to threads by function pthread_create();
 *  members: int id, the index of threads
 *           int* numTicketsp, points to numTickets  
 */  
struct tickAgent
{
    int id;
    int* numTicketsp;
};

/*
 * Start Routine of the thread, each thread gets it's own information
 * passed by *arg, and sells it's own  tikcets until tickets equals to 
 * zero when it is going to visit the critical region, it should get 
 * the lock first, iff it succeed in getting the lock, it consume numTickets, and
 * , report it sold a ticket and the tickets left, release the lock, 
 * and the leaves the critical region.
 *
 * parameters: void* arg, a generic pointer for passing information
 * return value: void* to tell the return status.
 */ 
void* sellTicket(void* arg)
{
    int numSoldByThisThread = 0;
    int id = ((struct tickAgent*)arg)->id;
    int* numTicketsp = ((struct tickAgent*)arg)->numTicketsp;

    while(1)
    {
        pthread_mutex_lock(&lock);
        //critical region
        if(*numTicketsp==0)
        {
            break;
        }
        --(*numTicketsp);
        printf("Agent %d sold a ticket,\t%d\t tickets left\n",id,*numTicketsp);
        ++numSoldByThisThread;
        // critical region 
        pthread_mutex_unlock(&lock);
        if(rand()%10>4)
        {
            usleep(10000);
        }
    }
    // what would happen if it breaks from while loop? 
    pthread_mutex_unlock(&lock);
    printf("Agent\t%d\t noticed all tickets sold!(I sold %d myself)\n", id, numSoldByThisThread);
    return NULL;
}

int main(int argc, char* argv[])
{
    // 1. random number seed
    srand((unsigned int )time(NULL));
    
    // 2. init the lock
    pthread_mutex_init(&lock,NULL);

    // 3. set threads attr  
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);


    // 4. create threads 
    pthread_t Agentid[NUMTICKAGENT];
    int numAgents =NUMTICKAGENT;
    struct tickAgent tickAgents[NUMTICKAGENT];   
    for(int i=0; i<16;++i)
    {
        tickAgents[i].id = i+1;
        tickAgents[i].numTicketsp = &numTickets;
        pthread_create(&Agentid[i],&attr, sellTicket,&tickAgents[i]);
    }
    
    // 5. threads join 
    for(int i=0; i<NUMTICKAGENT;++i)
    {
        pthread_join(Agentid[i],NULL);
    }

    printf("All Done!\n");
    return 0;
}


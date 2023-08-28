#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUMTICKAGENT 16
#define TICKETSTOSELL 150;

/*
 *  global shared resources: numTickets, num of tickets to sell
 *                           lock, mutex_t to protect numTickets
 */

int numTickets = TICKETSTOSELL;

/*
 *  using semaphore to limit the number of threads which can access to critical region
 */
sem_t lock;

/*
 *  information passed to threads by function pthread_create();
 *  members: int id, the index of threads
 *           int* numTicketsp, points to numTickets
 */

struct tickAgent
{
    int id;
    int *numTicketsp;
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
void *sellTicket(void *arg)
{
    int numSoldByThisThread = 0;
    int id = ((struct tickAgent *)arg)->id;
    int *numTicketsp = ((struct tickAgent *)arg)->numTicketsp;

    while (1)
    {
        sem_wait(&lock);
        // critical region
        if (*numTicketsp == 0)
        {
            break;
        }
        --(*numTicketsp);
        printf("Agent %d sold a ticket,\t%d\t tickets left\n", id, *numTicketsp);
        // critical region
        sem_post(&lock);
        ++numSoldByThisThread;
        if (rand() % 10 > 4)
        {
            usleep(1000);
        }
    }
    sem_post(&lock);
    printf("Agent %d sold %d tickets\n", id, numSoldByThisThread);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tickAgents[NUMTICKAGENT];
    struct tickAgent tickAgentInfo[NUMTICKAGENT];
    int i;
    srand(time(NULL));
    sem_init(&lock, 0, 1);
    for (i = 0; i < NUMTICKAGENT; ++i)
    {
        tickAgentInfo[i].id = i;
        tickAgentInfo[i].numTicketsp = &numTickets;
        pthread_create(&tickAgents[i], NULL, sellTicket, &tickAgentInfo[i]);
    }
    for (i = 0; i < NUMTICKAGENT; ++i)
    {
        pthread_join(tickAgents[i], NULL);
    }
    sem_destroy(&lock);
    return 0;
}
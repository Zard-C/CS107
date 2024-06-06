#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
    This example is a simulation of an ice cream store introduced in cs107 course at Stanford University.
    The store has a limited number of cones and scoops. The store has a limited number of customers.
    The store has a limited number of employees. The store has a limited number of cashiers.
*/

/*
    Since we don't have cs107 library, we use posix threads and semaphores to simulate the ice cream store.
*/

// global variables
struct Inspection
{
    bool passed;
    sem_t requested;
    sem_t finished; // granted is a better name
    sem_t lock;
} inspection;

// a FIFO queue
struct Line
{
    int number; // init as 0, increment by 1
    sem_t requested;
    sem_t customers[10];
    sem_t lock;
} line;

// a barrier to run all threads
pthread_barrier_t barrier;

void *clerk(void *arg);

void *customer(void *arg)
{
    int numCones = *(int *)arg;

    sem_t clerkSDone;
    sem_init(&clerkSDone, 0, 0);

    // wait for the barrier
    pthread_barrier_wait(&barrier);

    pthread_t clerkThreads[4];

    for (int i = 0; i < numCones; i++)
    {
        pthread_create(&clerkThreads[i], NULL, clerk, &clerkSDone);
        printf("Customer %lu is waiting for clerk to make cone %d\n", pthread_self(), i);
    }

    for (int i = 0; i < numCones; i++)
    {
        sem_wait(&clerkSDone);
        printf("Customer %lu has received cone %d\n", pthread_self(), i);
    }
    for (int i = 0; i < numCones; i++)
    {
        pthread_join(clerkThreads[i], NULL);
    }

    sem_destroy(&clerkSDone);

    // Walk to the cashier
    printf("Customer %lu is walking to the cashier\n", pthread_self());
    sem_wait(&line.lock);
    int position = line.number++; // get the position in line
    sem_post(&line.lock);

    printf("Customer %lu is at position %d in line\n", pthread_self(), position);
    sem_post(&line.requested);
    sem_wait(&line.customers[position]); // wait for the cashier to checkout
    printf("Customer %lu is done\n", pthread_self());
    return NULL;
}

void *clerk(void *arg)
{
    // sem_t semToSignal = *(sem_t *)arg;

    // make cones until passing inspection

    bool passed = false;
    while (!passed)
    {
        // Make a cone
        sem_wait(&inspection.lock);
        sem_post(&inspection.requested);
        sem_wait(&inspection.finished);
        passed = inspection.passed;
        sem_post(&inspection.lock);
    }

    // give the cone to customer
    printf("Clerk %lu has made a cone\n", pthread_self());
    sem_post((sem_t *)arg);

    return NULL;
}

void *manager(void *arg)
{
    int totalConesNeeded = *(int *)arg;
    int numApproved = 0;
    int numInspected = 0;

    // wait for the barrier
    pthread_barrier_wait(&barrier);

    while (numApproved < totalConesNeeded)
    {
        sem_wait(&inspection.requested);
        numInspected++;
        inspection.passed = rand() % 2;
        if (inspection.passed == true)
        {
            numApproved++;
        }
        sem_post(&inspection.finished);
    }

    return NULL;
}

void *cashier(void *arg)
{
    (void)arg;

    // wait for the barrier
    pthread_barrier_wait(&barrier);

    // there a total 10 customers in this simulation
    for (int i = 0; i < 10; i++)
    {
        sem_wait(&line.requested);
        // checkout the customer
        sem_post(&line.customers[i]);
    }

    return NULL;
}

void Initialize()
{
    sem_init(&inspection.requested, 0, 0);
    sem_init(&inspection.finished, 0, 0);
    sem_init(&inspection.lock, 0, 1);

    sem_init(&line.requested, 0, 0);
    sem_init(&line.lock, 0, 1);
    line.number = 0;
}

int main(int argc, char **argv)
{
    int totalCones = 0;
    srand(time(NULL));
    Initialize();

    // set a pthread attr to create a detached thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t customers[10];
    int cones[10];
    for (int i = 0; i < 10; i++)
    {
        cones[i] = rand() % 4 + 1;
        totalCones += cones[i];
    }

    pthread_barrier_init(&barrier, NULL, 12); // 10 customers, 1 manager, 1 cashier (12 threads in total

    for (int i = 0; i < 10; i++)
    {
        pthread_create(&customers[i], &attr, customer, &cones[i]);
    }

    pthread_t managerThread;
    pthread_create(&managerThread, NULL, manager, &totalCones);

    pthread_t cashierThread;
    pthread_create(&cashierThread, NULL, cashier, NULL);

    // Run all threads, how to implement this?

    // wait for all customers to finish
    for (int i = 0; i < 10; i++)
    {
        pthread_join(customers[i], NULL);
    }
    pthread_join(managerThread, NULL);
    pthread_join(cashierThread, NULL);
    pthread_barrier_destroy(&barrier);
    printf("Done simulation\n");

    return 0;
}
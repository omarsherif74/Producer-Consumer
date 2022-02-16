#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#define N 5   //number of counter threads
#define B 2   //fixed size of buffer

int shared=0; //global variable shared and accessed by threads
int empty; //current number of available places in buffer
int numInBuffer; //current number of full places in buffer
int sem_r_count; //number of threads allowed to access shared variable at a time

int queue[B]; //buffer
int qcount=0; //no. of elements in buffer


sem_t s,n,e,r;
//s : mutual exclusion semaphore to allow either monitor (producer) or collector (consumer) in critical section
//n : semaphore for collector to block if buffer is empty (keeps track of no. of full spaces in buffer)
//e : semaphore for producer (monitor) to block if buffer is full (keeps track of no. of empty spaces in buffer)
//r : allows 1 thread only to access shared variable at a time



void random_sleep(int min, int max)
{
    srand(time(NULL));  //sets time as seed for rand function
    sleep(rand()%((max+1)-min) + min);
}

void* counterFunc(int tid)
{
    while(1)
    {
        printf("Counter thread %d : received a message\n\n",tid);

        sem_getvalue(&r,&sem_r_count);

        if(sem_r_count<=0)
        {
            printf("Counter thread %d : waiting to write\n\n",tid);
        }


        sem_wait(&r); ///critical section

        ++shared;

        printf("Counter thread %d : now adding to counter, counter value = %d\n\n",tid,shared);

        sem_post(&r); ///critical section


        random_sleep(2,5);
    }
}

void* monitorFunc(void* arg)
{
    while(1)
    {

        sem_getvalue(&r,&sem_r_count);

        if(sem_r_count<=0)
        {
            printf("Monitor thread : waiting to read counter\n\n");
        }

        sem_getvalue(&e,&empty);

        if(empty==0)
        {
            printf("Monitor thread : BUFFER FULL!!\n\n");
        }

        sem_wait(&e);

        sem_wait(&s); ///critical section

        sem_wait(&r);

        int value=shared; //read counter

        printf("Monitor thread : reading a count value of %d\n\n",value);

        shared=0; //reset counter

        sem_post(&r);


        queue[qcount]=value;  //enqueue

        printf("Monitor thread : writing to buffer at position %d\n\n",qcount);

        qcount++;

        sem_post(&s); ///critical section

        sem_post(&n);

        random_sleep(3,3);
    }
}

void* collectorFunc(void* arg)
{
    while(1)
    {
        sem_getvalue(&n,&numInBuffer);

        if(numInBuffer<=0)
        {
            printf("Collector thread : NOTHING IS IN THE BUFFER!!\n\n");
        }

        sem_wait(&n);

        sem_wait(&s); ///critical section

        int x=queue[0];     //dequeue

        //shift all buffer elements to beginning of queue
        for(int j=0; j<qcount-1; j++)
        {
            queue[j]=queue[j+1];
        }

        qcount--;

        printf("Collector thread : reading from buffer at position %d\n\n",qcount);

        sem_post(&s); ///critical section

        sem_post(&e);

        random_sleep(3,5);
    }
}

void main()
{

    sem_init(&s,0,1);
    sem_init(&n,0,0);
    sem_init(&e,0,B);
    sem_init(&r,0,1);


    pthread_t counter[N];
    pthread_t monitor;
    pthread_t collector;

    for(int i=1; i<=N; i++)
    {
        int* thread_id=(int*)malloc(sizeof(int));
        thread_id=(int*)i;
        pthread_create(&counter[i],NULL,counterFunc,thread_id);
    }

    pthread_create(&monitor,NULL,monitorFunc,NULL);

    pthread_create(&collector,NULL,collectorFunc,NULL);



    for(int i=1; i<=N; i++)
    {
        pthread_join(counter[i],NULL);
    }

    pthread_join(monitor,NULL);

    pthread_join(collector,NULL);

}

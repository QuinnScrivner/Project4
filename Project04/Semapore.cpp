#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <semaphore.h>
using namespace std;
int sum;
sem_t sem_mutex;
int main(int argc, char *argv[])
{
    sem_init(&sem_mutex, 0, 1);
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, argv[1]);
    pthread_join(tid, NULL);
    printf("the sum is: %d\n", sum);
}
void *runner(void *param)
{
    int upper = atoi((char *)param);
    for (int i = 1; i <= upper; i++)
    {
        // Aquire
        //  pthread_mutex_lock(&sum_mutex);
        sum += i;
        // release
        //  pthread_mutex_uZnlock(&sum_mutex);
    }
}
/*
p1:
    stmts_1;
    signal(synch);
P2:
    wait(synch);
    stmts_2;
*/
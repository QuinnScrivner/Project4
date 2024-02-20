#include <stdlib.h>
#include <pthread.h>
#include "buffer.h"
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <iostream>
#include "buffer.cpp"
#include <vector>
using namespace std;
// named Semaphors
sem_t sem_empty;
sem_t sem_full;
// mutex lock
pthread_mutex_t mutex;
// variables for number of consumers
int nc;
// variable for number of producers
int np;
// variable for simulation time
int timeToTerm;
// variable for sleep time between threads
int sleepTime;
// buffer pointers
int in = 0;
int out = 0;
// help is for knowing if the user has entered yes or no
string help;
// Attr pointer
pthread_attr_t attr;
// the buffer in question using the value from the h file
buffer_item buffer[BUFFER_SIZE];
// Used in the end for figuring out how much was produced and consumed
int totalProduced;
int totalConsumed;
// Holders for figuring out how much was produced when.
vector<int> prodThread;
vector<int> consThread;
// Remainder in the buffer found via for loop.
int remain;
// Variable that will hold the amount of times the buffer is full or empty
int bufferFull;
int bufferEmpty;
// For use at the end, so my for loops don't go back to 1 i.e. Thread 10: -> Thread 11:
int threadNum;
// function that will print out everything that is in the buffer
void printBuffer()
{
    // formatting
    cout << "Buffers: ";
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        cout << ' ' << buffer[i] << ' ';
    }
    // Formatting
    cout << endl;
    cout << "          ";
    for (int i = 0; i < 5; i++)
    {

        if (i == in)
        {
            cout << "W";
        }
        if (i == out)
        {
            cout << "R";
        }
        if (i != in && i != out)
        {
            cout << "    ";
        }
    }
    cout << endl;
}
// Checking if what we are working with is prime or not.
bool isPrime(int n)
{
    // Corner case
    if (n <= 1)
        return false;

    // Check from 2 to n-1
    for (int i = 2; i <= n / 2; i++)
        if (n % i == 0)
            return false;

    return true;
}
void *consumer(void *param)
{
    while (1)
    {
        // Since Producer and Consumer threads were created virtually identically, I will be commenting on only one
        // First lock and wait for full.
        sem_wait(&sem_full);
        pthread_mutex_lock(&mutex);
        // if the arg, Yes, was sent, procede.
        if (help == "yes")
        {
            // Wait for the user provided amount of time.
            sleep(sleepTime);
            // Is the buffer empty/full.
            if (out == 4)
            {
                printf("\nAll buffers empty. Consumer %ld waits.\n\n", pthread_self() % 100);
            }
            // Check if the randomly generated number is Prime. Print out a special message.
            if (isPrime(buffer[out]))
            {
                printf("Consumer %ld reads %d * * * PRIME * * *\n", pthread_self() % 100, buffer[out]);
            }
            // Regular out put.
            else
            {
                printf("Consumer %ld reads %d \n", pthread_self() % 100, buffer[out]);
            }
            // Add one for every time the while loop runs. Added for the total later.
            totalConsumed += 1;
            // Vector updated so I can see specifically what was created.
            consThread.push_back(1);
            if (out == 0)
            {
                // If the buffer is empty/full, keep track and output it later.
                bufferEmpty++;
            }
            // Increment by 1%5, and then print the buffer.
            out = (out + 1) % 5;
            printBuffer();
        }
        else
        {
            // Do the critical section, then move on, no outputs
            sleep(sleepTime);
            totalConsumed += 1;
            consThread.push_back(1);
            if (out == 0)
            {
                bufferEmpty++;
            }
            out = (out + 1) % 5;
        }
        // Unlock the thread and then post empty.
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_empty);
        // Exit the thread.
        pthread_exit(0);
    }
}
void *producer(void *param)
{
    while (1)
    {
        // random number being created
        int random = rand() % 100;
        sem_wait(&sem_empty);
        pthread_mutex_lock(&mutex);
        if (help == "yes")
        {
            totalProduced += 1;
            prodThread.push_back(1);
            // Sleep for user generated amount of time.
            sleep(sleepTime);
            // what did the producer create.
            printf("Producer %ld writes: %d\n", pthread_self() % 100, random);
            buffer[in] = random;
            if (in == 4)
            {
                bufferFull++;
                printf("\nAll buffers full. Producer %ld waits.\n\n", pthread_self() % 100);
            }
            in = (in + 1) % 5;
            printBuffer();
        }
        else
        {
            // Sleep for user generated amount of time.
            sleep(sleepTime);
            totalProduced += 1;
            prodThread.push_back(1);
            buffer[in] = random;
            if (in == 4)
            {
                bufferFull++;
            }
            in = (in + 1) % 5;
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full);
        pthread_exit(0);
    }
}
int main(int argc, char *argv[])
{
    printf("Starting Threads...\n");
    // The next few lines are holders for user input.
    timeToTerm = atoi(argv[1]);
    sleepTime = atoi(argv[2]);
    np = atoi(argv[3]);
    nc = atoi(argv[4]);
    string yesNo = argv[5];
    // The holder help comes into play.
    help = yesNo;
    // Initalize the Semaphpores
    sem_init(&sem_empty, 0, 5);
    sem_init(&sem_full, 0, 0);
    // Initialize the Pthread Mutex
    pthread_mutex_init(&mutex, NULL);
    // Create the size of the pThreads
    pthread_t tid_p[np];
    pthread_t tid_c[nc];
    // Initalize the attr.
    pthread_attr_init(&attr);
    // Create the Threads, sleep, Join them together.
    for (int i = 0; i < np; i++)
    {
        pthread_create(&tid_p[i], &attr, producer, NULL);
    }
    for (int i = 0; i < nc; i++)
    {
        pthread_create(&tid_c[i], &attr, consumer, NULL);
    }
    sleep(timeToTerm);
    for (int i = 0; i < np; i++)
    {
        pthread_join(tid_p[i], NULL);
    }
    for (int i = 0; i < nc; i++)
    {
        pthread_join(tid_c[i], NULL);
    }
    //   Get command line arguments
    //   Initialize buffer
    //   Create producer thread(s)
    //   Create consumer thread(s)
    //   Sleep
    //   Join Threads
    //   Display Statistics
    //   Exit

    // STATISTICS.
    printf("\nPRODUCER / CONSUMER SIMULATION COMPLETE\n");
    printf("=======================================\n");
    printf("Simulation Time:                        %d\n", atoi(argv[1]));
    printf("Maximum Thread Sleep Time:              %d\n", atoi(argv[2]));
    printf("Number of Producer Threads:             %d\n", atoi(argv[3]));
    printf("Number of Consumer Threads:             %d\n", atoi(argv[4]));
    printf("Size of Buffer:                         %d\n\n", BUFFER_SIZE);
    printf("Total Number of Items Produced:         %d\n", totalProduced);
    for (int i = 1; i <= np; i++)
    {
        if (i >= 10)
        {
            printf("Thread %d:                              %d\n", i, prodThread[i]);
        }
        else
        {
            printf("Thread %d:                               %d\n", i, prodThread[i]);
        }
    }
    cout << endl;
    printf("Total Number of Items Consumed:         %d\n", totalConsumed);
    for (int i = 1; i <= nc; i++)
    {
        threadNum = np + i;
        if (threadNum >= 10)
        {
            printf("Thread %d:                              %d\n", threadNum, consThread[i]);
        }
        else if (threadNum >= 100)
        {
            printf("Thread %d:                             %d\n", threadNum, consThread[i]);
        }
        else
        {
            printf("Thread %d:                               %d\n", threadNum, consThread[i]);
        }
    }
    cout << endl;
    for (int i = 0; i < out; i++)
    {
        remain += i;
    }
    printf("Number Of Items Remaining in Buffer:    %d\n", remain);
    printf("Number Of Times Buffer Was Full:        %d\n", bufferFull);
    printf("Number Of Times Buffer Was Empty:       %d\n", bufferEmpty);
    // Finished.
}

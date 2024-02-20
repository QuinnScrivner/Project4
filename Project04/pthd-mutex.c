#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 100

int sum;
void *runner(void *param);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
  pthread_t tid[N];
  pthread_attr_t attr;

  if (argc != 2)
  {
    printf("usage: a.out <integer valude>\n");
    return -1;
  }

  pthread_attr_init(&attr);
  for (int i = 0; i < N; i++)
  {
    pthread_create(&tid[i], &attr, runner, argv[1]);
  }
  for (int i = 0; i < N; i++)
  {
    pthread_join(tid[i], NULL);
  }

  printf("Thread-Main: sum = %d\n", sum);
}

void *runner(void *param)
{
  int upper = atoi(param);
  //sum = 0;
  for (int i = 0; i <= upper; i++)
  {
    pthread_mutex_lock(&mutex);
    sum += i;
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(0);
}

#include <pthread.h>//
#include <stdlib.h>
#include <printf.h>
#include <semaphore.h>

volatile int cnt = 0; /* global */
sem_t mutex;



void *thread(void *vargp)
{ int i, niters = *((int *)vargp);
    for (i = 0; i < niters; i++){
        sem_wait(&mutex); /* P(s) */
        cnt++;
        sem_post(&mutex);}
    return NULL;
}

int main(int argc, char **argv)
{
    sem_init(&mutex, 0, 1);
    /* V(s) */
    int niters = 10000;
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread, &niters);
    pthread_create(&tid2, NULL, thread, &niters);
    pthread_join(tid1, NULL); pthread_join(tid2, NULL);
/* Check result */
    if (cnt != (2 * niters))
        printf("BOOM! cnt=%d\n", cnt);
    else{
        printf("OK cnt=%d\n", cnt); }
    exit(0);}
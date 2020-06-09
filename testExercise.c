#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h> 
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/types.h>
#include <sys/select.h>

#include <stdint.h> 
#include <pthread.h>

#include <time.h>
#include <signal.h>


pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;

int platform1;
int platform2;
int platform3;
//https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
int volatile status;
pthread_t trains[7];


struct data{
    int trainNr;
    int arrivalTime;
    int stopTime;
};

void* trainThread(void* arg){
    //thread method

    struct data* dataArg = (struct data*) arg;
    time_t start;
    time_t diff;
    time(&start);
    sleep(dataArg->arrivalTime);
    

    
    while (1)
    {
        if(pthread_mutex_trylock(&lock1) == 0){
            diff = difftime(time(NULL),start);
            printf("%ld seconds after the start: train %d moves into the station at platform %d\n",diff,dataArg->trainNr,1);
            platform1 = dataArg->trainNr;
            sleep(dataArg->stopTime);
            diff = difftime(time(NULL),start);
            printf("%ld seconds after the start: train %d leaves the station at platform %d\n",diff,dataArg->trainNr,1);
            platform1 = 0;
            pthread_mutex_unlock(&lock1);
            break;

        }

        if(pthread_mutex_trylock(&lock2) == 0){
            diff = difftime(time(NULL),start);
            printf("%ld seconds after the start: train %d moves into the station at platform %d\n",diff,dataArg->trainNr,2);
            platform2 = dataArg->trainNr;
            sleep(dataArg->stopTime);
            diff = difftime(time(NULL),start);
            printf("%ld seconds after the start: train %d leaves the station at platform %d\n",diff,dataArg->trainNr,2);
            platform2 = 0;
            pthread_mutex_unlock(&lock2);
            break;
        }

        if(pthread_mutex_trylock(&lock3) == 0){
            diff = difftime(time(NULL),start);
            printf("%ld seconds after the start: train %d moves into the station at platform %d\n",diff,dataArg->trainNr,3);
            platform3 = dataArg->trainNr;
            sleep(dataArg->stopTime);
            diff = difftime(time(NULL),start);
            printf("%ld seconds after the start: train %d leaves the station at platform %d\n",diff,dataArg->trainNr,3);
            platform3 = 0;
            pthread_mutex_unlock(&lock3);
            break;
        }
    }


    pthread_exit(arg);

}

void interrupt_Handler(){
  status = 1;
}

int main(int argc, char const *argv[])
{
    status = 0;
    pthread_mutex_init(&lock1,NULL);
    pthread_mutex_init(&lock2,NULL);
    pthread_mutex_init(&lock3,NULL);

    sigset_t mask;
    sigemptyset (&mask); //create an empty set
    struct sigaction saSig1;
    saSig1.sa_handler = interrupt_Handler;
    saSig1.sa_mask = mask;
    saSig1.sa_flags = SA_INTERRUPT;
    sigaction(SIGINT, &saSig1, NULL);


    struct data* data[7];

    data[0] = malloc(sizeof(struct data)); 
    data[0]->trainNr = 1;
    data[0]->arrivalTime = 0;
    data[0]->stopTime = 8;
    data[1] = malloc(sizeof(struct data)); 
    data[1]->trainNr = 2;
    data[1]->arrivalTime = 0;
    data[1]->stopTime = 4;
    data[2] = malloc(sizeof(struct data)); 
    data[2]->trainNr = 3;
    data[2]->arrivalTime = 2;
    data[2]->stopTime = 3;
    data[3] = malloc(sizeof(struct data)); 
    data[3]->trainNr = 4;
    data[3]->arrivalTime = 3;
    data[3]->stopTime = 7;
    data[4] = malloc(sizeof(struct data)); 
    data[4]->trainNr = 5;
    data[4]->arrivalTime = 5;
    data[4]->stopTime = 3;
    data[5] = malloc(sizeof(struct data)); 
    data[5]->trainNr = 6;
    data[5]->arrivalTime = 5;
    data[5]->stopTime = 5;
    data[6] = malloc(sizeof(struct data)); 
    data[6]->trainNr = 7;
    data[6]->arrivalTime = 6;
    data[6]->stopTime = 2;


    for (int i = 0; i < 7; i++)
    {
        pthread_create(&trains[i],NULL,&trainThread,data[i]);

    }

    int finished = 0;
    //main thread waits for all other threads
    while(finished < 7)
    {
        if(pthread_tryjoin_np(trains[finished],NULL) == 0){
            finished++;
        }
        if(status == 1){
            printf("Platform 1: Train %d \n Platform 2: Train %d \n Platform 3: Train %d\n",platform1,platform2,platform3);
            for (int i = 0; i < 7; i++)
            {
                pthread_cancel(trains[i]);
            }
            break;
        }


    }
    for (int i = 0; i < 7; i++)
    {
        free(data[i]);
    }

    pthread_mutex_destroy(&lock1);  
    pthread_mutex_destroy(&lock2);    
    pthread_mutex_destroy(&lock3);

    return EXIT_SUCCESS;
}

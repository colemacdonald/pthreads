/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */

int             group_size;
int             meet_order;
pthread_mutex_t m;
pthread_cond_t  barrier_q;
resource_t      code_word;

int             count;
int             gen;


/*
 * Initialize all variables above.
 * Set count and gen to 0
 */
void initialize_meetup(int n, int mf) {
    char label[100];
    int i;

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */

     group_size = n;
     meet_order = mf;

     count = 0;
     gen = 0;

     init_resource(&code_word, "code");

     if(pthread_mutex_init(&m, NULL) != 0)
     {
        fprintf(stderr, "Could not initialize mutex 'm'\n");
        exit(0);
     }
     if(pthread_cond_init(&barrier_q, NULL) != 0)
     {
        fprintf(stderr, "Could not initialize condition variable 'barrier_q'\n");
     }
}


void join_meetup(char *value, int len) {
    //printf("NOTHING IMPLEMENTED YET FOR join_meetup\n");
    pthread_mutex_lock(&m);
    count++;

    if( (count == 1 && meet_order == MEET_FIRST) || (count == group_size && meet_order == MEET_LAST) )
    {
        write_resource(&code, value, len);
    }

    if(count < group_size)
    {
        int my_gen = gen;
        while(my_gen == gen)
            pthread_cond_wait(&barrier_q, &m);
    }
    else
    {
        count = 0;
        generation++;
        pthread_cond_broadcast(&barrier_q);
    }
    pthread_mutex_unlock(&m);
}

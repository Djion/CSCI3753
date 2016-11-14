/* Programming Assignment 3
 * Alex Ring
 * Fall 2016 CSCI 3753
 */


/* Standard stuff to import, includes the stuff for pthreads*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>


#include "util.h" //DNS lookup implementation, provided
#include "queue.h" //Queue lookup implementation, provided

/* Some limits suggested by the PDF and taken from Lookup.c */
#define MAX_IP_LENGTH INET6_ADDRSTRLEN //Limits how long an ip can be
#define MINARGS 3 //From lookup.c
#define USAGE "<inputFilePath> <outputFilePath>" //From lookup.c
#define SBUFSIZE 1025 //From lookup.c
#define INPUTFS "%1024s" //From lookup.c
#define QUEUE_SIZE 10 //Size of queue
#define MAX_THREADS 10 //Number of requester threads
#define MAX_RESOLVER_THREADS 10 //Number of resolver threads

/* Create Mutexes to block with
 * mutex = General blocking mutex
 * queuePushMutex = Mutex to block from adding to the queue
 * queueReadMutex = Mutex to block from reading from the queue
 * queueMutex = Block on entire queue
 * outputFileMutex = Block on output file
 */
pthread_mutex_t mutex;
pthread_mutex_t queuePushMutex;
pthread_mutex_t queueReadMutex;
pthread_mutex_t queueMutex;
pthread_mutex_t outputFileMutex;

int requestThreadsComplete = 0;
int readCount = 0;

/* Data structure for the request and resolver threads*/
struct RequesterThread{
    long threadNumber;
    FILE* inputFile;
    queue* q;
};
struct ResolverThread{
    FILE* outputFile;
    queue* q;
};
void* Request(void* threadarg){
    /*Declare the thread */
    struct RequesterThread * requester;
    char hostname[SBUFSIZE];
    FILE* inputfp;
    queue* q;
    char* domainName;
    long threadNumber;


    /*Grab the data from the thread */
    requester = (struct RequesterThread *) threadarg;
    inputfp = requester->inputFile;
    q = requester->q;
    threadNumber = requester->threadNumber;

    
    /* Error handling
     * Given a bad output file path, your program should exit and
     * print an appropriate error to stderr
     */
    if(!inputfp){
        fprintf(stderr, "Bogus Output File Path\n");
        pthread_exit(NULL);
    }

    /* Add the hostnames/ips to the queue
     * this needs to be a protected process
     */
    printf("Reading from the file\n");

    /* Loop through the unput file */

    while(fscanf(inputfp, INPUTFS, hostname) > 0){
        /* Condition Variable */
        int completed = 0;

        while(!completed){
            /* Need to block other threads from reading from the queue
             * and from adding to the queue. While we check if queue
             * is full. 
             */
            pthread_mutex_lock(&queueReadMutex);
            pthread_mutex_lock(&queuePushMutex);

            /* Check if the queue is full */
            if (!queue_is_full(q)){

                /* create variable to hold the string to copy */
                domainName = malloc(SBUFSIZE);

                /* char *strncpy(char *dest, const char *src, size_t n)
                 * copies up to n characters from the string pointed to,
                 * by src to dest. If its smaller than size, pad with null
                 *https://www.tutorialspoint.com/c_standard_library/c_function_strncpy.htm
                 */
                strncpy(domainName, hostname, SBUFSIZE);

                printf("Hostname Added: %s\n", domainName);

                /* Use the queue to push the domain name info to the queue */
                queue_push(q, domainName);

                /* We are done, change condition variable */
                completed = 1;
            }
            /* Now that we are no longer doing anything with the queue we can
             * allow it to be used by other threads.
             */
            pthread_mutex_unlock(&queuePushMutex);
            pthread_mutex_unlock(&queueReadMutex);

            /* If we are not completed, let's wait */
            if (!completed){
                usleep((rand()%100)*100000);
            }
        }
    }
    /* Close the input file */
    fclose(inputfp);

    /* Thread is done now */
    printf("Thread Complete: %ld\n", threadNumber);
	
    return NULL;
}

void* Resolve(void* threadarg){
    /* Declare the thread */
    struct ResolverThread * resolver;
    FILE* outputfp;
    /* PDF says we should use MAX_IP_LENGTH = 1025 */
    char firstipstr[MAX_IP_LENGTH];
    char * hostname;
    queue* q;


    resolver = (struct ResolverThread *) threadarg;
    outputfp = resolver->outputFile;
    q = resolver->q;

    /* Condition variable to check if the queue is empty
     * we want to stop resolving if the queue is empty
     */
    int emptyQueue = 0;

    /* Is the queue empty? Are we done requesting threads?*/
    while (!emptyQueue || !requestThreadsComplete){
    	int resolved = 0;

        /*Going to change a conditional variable involving number of read
         * hostnames. Since this is a variable used by other threads,
         * need to block while we change it */
        pthread_mutex_lock(&queueReadMutex);
        pthread_mutex_lock(&mutex);
        readCount++;

        /* We have started reading from the queue. We need to block threads
         * from adding to the queue while we are reading from it
         */
        if (readCount==1){
            pthread_mutex_lock(&queuePushMutex);
        }

        /* We are done modifying and comparing against the readCount conditional
         * we can unlock the queue to be read by other resolver threads
         */
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&queueReadMutex);

        /* Now we need to get some stuff from the queue, lets block it*/
        pthread_mutex_lock(&queueMutex);

        /* Is the queue empty? We don't need to do anything if it is */
        emptyQueue = queue_is_empty(q);
        if(!emptyQueue){
            hostname = queue_pop(q);
            if (hostname != NULL){
            	
            	printf("Hostname Read: %s\n", hostname);
            	
	            resolved = 1;
        	}
        }

        /* We are done popping stuff off of the queue, no need to block it anymore*/
        pthread_mutex_unlock(&queueMutex);

        /* We are going to edit the conditional variable again. Lets block */
        pthread_mutex_lock(&mutex);

        /* If read is 0, all the threads to this point have finsihed popping off the queue
         * we can unblock the requester threads so they can write to the queue again
         */
        readCount--;
        if (readCount == 0){
            pthread_mutex_unlock(&queuePushMutex);
        }

        /* Done editing the conditional, we can unblock */
        pthread_mutex_unlock(&mutex);

        /* If we resolved all the crap above, let's call lookup and get the info
         * that we need to output to the file.
         */
        if(resolved){
            /*Call the dnslookup function from util, provided by the assignment
             *Need to handle and print error just in case the IP lookup fails
             */
            if(dnslookup(hostname, firstipstr, sizeof(firstipstr)) == UTIL_FAILURE){
                fprintf(stderr, "Bogus Hostname: %s\n", hostname);
                strncpy(firstipstr, "", sizeof(firstipstr));
            }
            /* Now we are going to write to the output file, lets block the other
             * threads from writing to this file until we are done. 
             */
    		pthread_mutex_lock(&outputFileMutex);

            /* Write the hostname and ip*/
            fprintf(outputfp, "%s", hostname);
            fprintf(outputfp, ",%s",firstipstr);
		    fprintf(outputfp, "\n");

            /*We are done, can stop blocking the other threads now*/
            pthread_mutex_unlock(&outputFileMutex);

            /*free hostname to prevent memory leak*/
		    free(hostname);
        }

    }

    
    printf("Complete Resolve thread\n");
    

    return NULL;
}


int main(int argc, char* argv[]){
    /*A lot of the code below is taken from the lookup file that
     *we were provided*/

	/* Local Vars */
    FILE* outputfp = NULL;

    /* Check Arguments */
    if(argc < MINARGS){
		fprintf(stderr, "Not enough arguments: %d\n", (argc - 1));
		fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
		return EXIT_FAILURE;
    }
    if (argc > MAX_THREADS + 1){
    	fprintf(stderr, "Too many files: %d\n", (argc - 2));
    	fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
		return EXIT_FAILURE;
    }

    /* Open Output File */
    outputfp = fopen(argv[(argc-1)], "w");
    if(!outputfp){
		perror("Error Opening Output File");
		return EXIT_FAILURE;
    }

    /* Need to build the queue that we are going to be using */
    queue q;
    const int qSize = QUEUE_SIZE;
    if(queue_init(&q, qSize) == QUEUE_FAILURE){
		fprintf(stderr, "Queue failed to intialize\n");
    }


    /* Now we need to make all of the threads for the requests
     * Decalre the threads based on max_threads
     */
    struct RequesterThread requester[MAX_THREADS];
    pthread_t requestThreads[MAX_THREADS];
    int rc;
    long t;

    /* Make em all, based on pthread hello file we are given */
    for(t=0; t<(argc-2) && t<MAX_THREADS; t++){
        requester[t].q = &q;
        requester[t].inputFile = fopen(argv[t+1], "r");
        requester[t].threadNumber = t;
        
        printf("Creating %ld Requester Thread\n", t);
        
		rc = pthread_create(&(requestThreads[t]), NULL, Request, &(requester[t]));
		if (rc){
		    printf("ERROR; return code from pthread_create() is %d\n", rc);
		    exit(EXIT_FAILURE);
		}
    }


    /* Now we need to make all of the resolver threads
     * Also declaring these using max_threads */
    struct ResolverThread resolver;
    pthread_t resolveThreads[MAX_RESOLVER_THREADS];
    int rc2;
    long t2;

    resolver.q = &q;
    resolver.outputFile = outputfp;

    /* Make em all, based on pthread hello file we are given */
    for(t2=0; t2<MAX_RESOLVER_THREADS; t2++){
    	
    	printf("Creating %ld Resolver Thread\n", t2);
    	
        rc2 = pthread_create(&(resolveThreads[t2]), NULL, Resolve, &resolver);
        if (rc2){
            printf("ERROR; return code from pthread_create() is %d\n", rc2);
            exit(EXIT_FAILURE);
        }
    }

    /* Join all of the requester threads to complete */
    for(t=0; t<(argc-2) && t<MAX_THREADS; t++){
        pthread_join(requestThreads[t],NULL);
    }
    
    /* Set conditional variable used in the resolver thread*/
    requestThreadsComplete = 1;

    /* Join all of the resolver threads to complete */
    for(t=0; t<MAX_RESOLVER_THREADS; t++){
        pthread_join(resolveThreads[t],NULL);
    }
    
    /* Close Output File */
    fclose(outputfp);

    /* Cleanup Queue */
    queue_cleanup(&q);

    printf("\n\nAll threads have completed, file has been written. GG\n");
    return EXIT_SUCCESS;
}

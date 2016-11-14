/*Modified pi-sched.c to fork the process somewhere between 5 and 10 times, chosen at random
 it prints out the child ID and finishes the execution.*/

/*
 * File: pi-sched.c
 * Author: Andy Sayler
 * Revised: Dhivakant Mishra
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: 2012/03/07
 * Modify Date: 2012/03/09
 * Modify Date: 2016/31/10
 * Description:
 * 	This file contains a simple program for statistically
 *      calculating pi using a specific scheduling policy.
 */

/* Local Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sched.h>

#define DEFAULT_ITERATIONS 1000000
#define RADIUS (RAND_MAX / 2)
#define DEFAULT_OUTPUT_BASE "mixedOutput"

static inline double dist(double x0, double y0, double x1, double y1){
    return sqrt(pow((x1-x0),2) + pow((y1-y0),2));
}

static inline double zeroDist(double x, double y){
    return dist(0, 0, x, y);
}

int main(int argc, char* argv[]){

    long i;
    long iterations;
    struct sched_param param;
    int policy;
    double x, y;
    double inCircle = 0.0;
    double inSquare = 0.0;
    double pCircle = 0.0;
    double piCalc = 0.0;
    int mixedPriority = 0;
    int processNumber;
    /* Process program arguments to select iterations and policy */
    /* Set default iterations if not supplied */
    if(argc < 2){
	iterations = DEFAULT_ITERATIONS;
    }
    /* Set default policy if not supplied */
    if(argc < 3){
	policy = SCHED_OTHER;
    }
    /* Set iterations if supplied */
    if(argc > 1){
	iterations = atol(argv[1]);
	if(iterations < 1){
	    fprintf(stderr, "Bad iterations value\n");
	    exit(EXIT_FAILURE);
	}
    }
    /* Set policy if supplied */
    if(argc > 2){
	if(!strcmp(argv[2], "SCHED_OTHER")){
	    policy = SCHED_OTHER;
	}
	else if(!strcmp(argv[2], "SCHED_FIFO")){
	    policy = SCHED_FIFO;
	}
	else if(!strcmp(argv[2], "SCHED_RR")){
	    policy = SCHED_RR;
	}
	else{
	    fprintf(stderr, "Unhandeled scheduling policy\n");
	    exit(EXIT_FAILURE);
	}
    }
    
    /* Set conditional for using mixed priorities */
    if(argc > 3){
	mixedPriority = atol(argv[3]);
    }

    if(argc > 4){
	if(!strcmp(argv[4], "LOW")){
	processNumber = 0;
	}
	else if(!strcmp(argv[4], "MED")){
	processNumber = 1;
	}
	else if(!strcmp(argv[4], "HIGH")){
	processNumber = 2;
	}
	else{
		fprintf(stderr, "Invalid Number of Processes Choice\n");
		exit(EXIT_FAILURE);
	}
    }
	
    
    /* Fork processes based on conditionals*/
    /* Pick a random number between 5-10*/

    srand(time(NULL));
    int r;
    
    if(processNumber == 0){
    	r = rand() % 10;
    	if(r< 5){
		r = 5;}
    }
    else if(processNumber == 1){
	r = rand() % 100;
	if(r < 10){
		r = 10;}
    }
    else{
	r = rand() % 1000;
	if(r < 100){
		r = 100;}
    }
	
    int j, pid;

    for(j = 0; j<r; j++){
            pid = fork();
            if(pid == 0){
                    printf("CHILD (%d): %d\n", j + 1, getpid());
 
	/*Pick a random priority*/
	srand(time(NULL));
	int randomPriority;
	randomPriority = rand() % 100;
	if(randomPriority == 0){
		randomPriority = 1;
	}
	printf("RANDOM PRIORITY %d \n",randomPriority);

    	/* Set process to max prioty for given scheduler if we are not using mixed, otherwise set the priority randomly */
/* Set process to max prioty for given scheduler */
    	if(mixedPriority == 0){
    	param.sched_priority = sched_get_priority_max(policy);
    	}
    	else if (mixedPriority == 1 && policy == SCHED_RR){
    	param.sched_priority = randomPriority;
    	}
    	else if (mixedPriority == 1 && policy == SCHED_FIFO){
    	param.sched_priority = randomPriority;
	    }

	else{ //Have to do the nice value for CFS different than the other ones
	srand(time(NULL));
	int randomNice;
	randomNice = rand() % 20;
	nice(randomNice);
	}
    
    	/* Set new scheduler policy 	*/
    	fprintf(stdout, "Current Scheduling Policy: %d\n", sched_getscheduler(0));
    	fprintf(stdout, "Setting Scheduling Policy to: %d\n", policy);
    	if(sched_setscheduler(0, policy, &param)){
		perror("Error setting scheduler policy");
		exit(EXIT_FAILURE);
    	}
    	fprintf(stdout, "New Scheduling Policy: %d\n", sched_getscheduler(0));


    	/* Calculate pi using statistical methode across all iterations*/
    	FILE *fp;
        char fileName[30];
        int pidpid;
        pidpid = getpid();
        sprintf(fileName, "mixedOutput%d",pidpid);

        for(i=0; i<iterations; i++){
		x = (random() % (RADIUS * 2)) - RADIUS;
		y = (random() % (RADIUS * 2)) - RADIUS;
		
        fp = fopen(fileName, "w+");
        fprintf(fp, "pi = %f\n", x);
        fclose(fp);

        if(zeroDist(x,y) < RADIUS){
	    	inCircle++;
		}
		inSquare++;
    	}

    	pCircle = inCircle/inSquare;
    	piCalc = pCircle * 4.0;

        
        fp = fopen(fileName, "w+");
    	fprintf(fp, "pi = %f\n", piCalc);
        fclose(fp);
    	return 0;
	}
}
while (pid = waitpid(-1, NULL, 0)){
        if (errno == ECHILD){
                break;
        }
}
return 0;
}

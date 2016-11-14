/* Header file for multi-lookup.c
 * Alex Ring
 * Fall 2016 CSCI 3753
 */

#ifndef MULTI-LOOKUP_H
#define MULTI-LOOKUP_H
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "util.h"
#include "queue.h"


void* Request(void* threadarg);
void* Resolve(void* threadarg);

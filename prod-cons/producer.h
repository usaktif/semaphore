
#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include "common.h"

// prototype(s)
void removesem (int sid);
void produce_item (char* item);
void down (int sid, int semaphore);
void up (int sid, int semaphore);
void enter_item (char item, char* segptr, int* slot);
int ask (void);

void createshm (int* shmid, char** segptr, key_t key);
void removeshm (int shmid);

#endif


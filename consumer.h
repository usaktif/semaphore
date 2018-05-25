
#ifndef _CONSUMER_H_
#define _CONSUMER_H_

#include "common.h"

// prototype(s)
void removesem (int sid);
void consume_item (char);
void down (int sid, int semaphore);
void up (int sid, int semaphore);
void remove_item (char* item, char* segptr, int* slot);
int ask (void);

void createshm (int* shmid, char** segptr, key_t key);
void removeshm (int shmid);
void showshm (char* segptr);
#endif


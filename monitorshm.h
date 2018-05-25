
#ifndef _MONITORSHM_H_
#define _MONITORSHM_H_

#include "common.h"

// prototype(s)
int ask (void);

void attachshm (int* shmid, char** segptr, key_t key);
void showshm (char* segptr);
#endif


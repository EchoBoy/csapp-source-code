//
// Created by ligand on 2019-04-28.
//
#include <unistd.h>
#ifndef CSAPP_9_9_12_MM_H_
#define CSAPP_9_9_12_MM_H_

// 对应 malloc
void *mm_malloc(size_t size);

// 对应 free
void mm_free(void *ptr);
#endif //CSAPP_9_9_12_MM_H_

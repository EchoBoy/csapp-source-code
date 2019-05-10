//
// Created by ligand on 2019-04-12.
//

/**
 * 使用动态链接来替代 标准c库 中的 malloc 和 free
 */

#ifdef RUNTIME
#define _GUN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static int malloc_size = 0;


void *malloc(size_t size)
{
  void *(*mallocp)(size_t size);
  char *error;
  mallocp = dlsym(RTLD_NEXT, "malloc");
  if ((error = dlerror()) != NULL) {
    fputs(error, stderr);
    exit(1);
  }
  char *ptr = mallocp(size);
  malloc_size += (int)size;
  printf("malloc(%d) = %p, total=%d", (int)size, ptr, malloc_size);
  return ptr;
}

void free(void *ptr)
{
  void (*freep)(void *) = NULL;
  char *error;
  if (!ptr) return;
  freep = dlsym(RTLD_NEXT, "free");
  if ((error = dlerror()) != NULL ) {
    fputs(error, stderr);
    exit(1);
  }
  freep(ptr);
  printf("free(%p), total=%d", ptr, malloc_size);
}
#endif

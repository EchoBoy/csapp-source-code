//
// Created by ligand on 2019-04-12.
//


#include <stdio.h>
#include <dlfcn.h>

int main() {
  void *handle;
  void *(*mallocp)(size_t size);
  char *error;

  handle = dlopen("./mymalloc.so", RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "%s\n", dlerror());
    return 1;
  }

  mallocp = dlsym(handle, "malloc");
  if ((error = dlerror()) != NULL) {
    fprintf(stderr, "%s\n", error);
    return 1;
  }
  int *p = mallocp(32);

  return 0;
}

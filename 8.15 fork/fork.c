#include <sys/types.h>
#include <unistd.h>
#include "../csapp.h"


  
int main() {
  int n = 10;
  for (int i = 0; i < n; i++){
    sleep(1);
    Fork();
  }
  sleep(10);
  return 0;
}

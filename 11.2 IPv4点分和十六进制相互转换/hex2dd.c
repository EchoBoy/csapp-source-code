#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {

  if (argc < 2){
    printf("useage: hex2dd <hex_ip_addr>\n");
    return 0;
  }

  char *o_src  = argv[1];
  char res[16];
  char *end;

  long tmp = strtol(o_src, &end, 16);
  uint32_t src = htonl(tmp);
  // little-endian to big-endian

  if (*end) {
    printf("%s is not a hex number", o_src);
    return 0;
  }

  const char *r = inet_ntop(AF_INET, &src, res, 15);
  if (r == NULL)
    printf("error hex_ip_addr: %s\n", argv[1]);
  else
    printf("%s\n", res);
  return 0;
}

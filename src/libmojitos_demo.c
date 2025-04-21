#include <stdint.h>
#include <stdio.h>

#include "libmojitos.h"

int main(int argc, char **argv)
{
  int n = moj_init(argv);

  if(argc==1 || n <= 0)
    moj_usage();
  
  char **l = moj_labels();
  for (int i = 0; i < n; i++){
    printf("%s\t", l[i]);
  }
  printf("\n");
  
  const uint64_t* values = moj_get_values();
  for (int i = 0; i < n; i++){
    printf("%ld\t", values[i]);
  }
  printf("\n");

  moj_clean();
  return 0;
}

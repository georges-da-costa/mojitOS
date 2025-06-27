
/*

To compile
  gcc libmojitos_demo.c -o libmojitos_demo -I/usr/local/include/ -I/usr/include -lmojitos -llikwid

The last one comes from the options
  make options

*/
  
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <libmojitos.h>

int main(int argc, char **argv)
{
  int n = moj_init(argv);

  if(argc==1 || n <= 0) {
      printf("Usage\n");
      exit(0);
  }
  
  char **l = (char**) moj_labels();
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

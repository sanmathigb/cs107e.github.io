#include <stdio.h>
#include <limits.h>
#include <assert.h>

int main(void) {
  assert(sizeof(unsigned char)==1);
  
  unsigned char uc1 = 0xff;
  unsigned char uc2 = 0;
  
  if(~uc1 == uc2) {
    printf("%hhx == %hhx\n", ~uc1, uc2);
  } else {
    printf("%hhx != %hhx\n", ~uc1, uc2);
  }
  return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "time.h"


int nextTime(float rate_param)
{
  //  return (int) -logf(1.0f - (float) random()/ RAND_MAX) / rateParameter;
   int r =  rand();
   float result = -logf(1.0f - (float) r / RAND_MAX) / rate_param;
   return (int) result;

}

int main() {
  srand(time(NULL));
  int const MAX_EPOCH = 60;
  float rate_parameter = 1.0/6.0;
  int i = 0; int count = 0; int epoch = 0 ;
  int next = 1;
  next  = nextTime(rate_parameter);
  printf("%d next poisson \n", next);

  while ( epoch <= MAX_EPOCH ){
    printf("%d count \n", count);
    if (count == next){
      printf("Run Gossip algorithm\n" );
      next  = nextTime(rate_parameter);
      printf("%d next poisson \n", next);
      count = 0;
      epoch++;
    }else{
      epoch ++;
      count++;
    }

  }   // end loop
  return 0;
}

// make queries in different nodes - to speed up the process
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "time.h"

/*
the variable state contain the actual color of LED
possible values:
RED(r), GREENg), BLUE(b)
*/
char state;
/*
active = 1 mean that node is active and will run the vote model
inactive = 0 mean that node is inactive waiting a time slot
initially every node should be in inactive mode
*/
int active = 0;

/*
block 1 mean that the node can't receive a request because it's already
in communication with other node
block 0 mean that the node is accepting requests
*/
int block = 0;


char neighbors [4][18] = {"0013A200414F7FC2","0013A200414F7FC3","0013A200414F7FC4","0013A200414F7FC5"};
char colour_request[] = "COLOUR_REQUEST";


int nextTime(float rate_param)
{
  //  return (int) -logf(1.0f - (float) random()/ RAND_MAX) / rateParameter;
   int r =  rand();
   float result = -logf(1.0f - (float) r / RAND_MAX) / rate_param;
   return (int) result;

}

/*
get a random number between 0 and n - 1
*/
int get_neighbor(int n){
  int randomnumber;
  randomnumber = rand() % n;
  printf("%dchoosen neighbor\n", randomnumber);
  return randomnumber;
}
/*
Send message to neighbor node to request his state value
need to define the message format
*/
void send_message (char  address[], char [] request){//

  printf("dest addr %s \n", address);
  printf("status %s \n", request);
  // send(address,status)
}

void receive_message(){
  /*
    read message from serial monitor
    if (message recieved is COLOUR_REQUEST)
      send status to address
    else
      discard message
  */


}

/* In this method the node should read the value of
the other node and change the state to the read value
For the moment the new state is sent by parameter
*/
void update_state(char address[], char new_value){
     // maybe we need some verification here
     state = new_value;
     printf("new value %s\n", state);
     }


int main() {
  srand(time(NULL));
  int const MAX_EPOCH = 60;
  float rate_parameter = 1.0/6.0;
  int i = 0; int count = 0; int epoch = 0 ;
  int next = 1;        // next iteration
  int n = 4;           // number of nodes
  int index=0;         // index of next neighbor to connect
  unsigned long timeout = 500;
  // obtein next loop to be active
  next  = nextTime(rate_parameter);
  printf("%d next poisson \n", next);
  /*
    Until this point everything have to be
    in the setup method
  */

  while ( epoch <= MAX_EPOCH ){

    /*
       listen if I have some message to receive
        and reply:  send_message (char  address[], char status)
        Node can only listen in the first part of the loop
    */
    printf("%d count \n", count);
    if (count == next){
      block = 1;          // probably not needed

      printf("Run Gossip algorithm\n" );

      index = get_neighbor(n);
      printf("neig addr: %s\n", neighbors[index]);
      send_message(neighbors[index],colour_request);
      /* wait for neighbor reply or timeout
      unsigned long StartTime = millis();
      while (reply of timeout){
        sleep(sometime);
        if neigbor_reply
          then change state to neigbor state
          update_state(char address, new_value)
       else
         unsigned long CurrentTime = millis();
         unsigned long ElapsedTime = CurrentTime - StartTime;
          if (ElapsedTime>=timeout)
            continue;
      }
      */


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

#include <SoftwareSerial.h>

int AT_delay = 100;
int cord = 3;
String inputString = "";
String recvString = "";
String procString = "";
String type = "";
String rec_addr = "";
String TX_dest_addr = "";
String rec_data = "";
String rec_block = "";
String source_addr = "";
String data = "";
String block = "";
boolean active_mode = false;   // Normally every node is in inActive mode

boolean SIR = false;

int counter = 0;
boolean start_timer = false;

//for line 
String My_ID = "3A";

//LINE & CIRCLE
int n = 2;           // number of neighbor
String neighbors [2] = { "3C","C2"};

//for complete topology
//int n=6;
//String neighbors[6]={"3C","CC","C2","3D","3E","23"};

//for grid
//int n=3;
//String neighbors[3]={ "C2","3C"};


long randNumber;
int R = 6;
int B = 7;
int G = 8;
int state = 0;

int time_out_constant = 2500;

unsigned long startTime;
unsigned long currentTime;

unsigned long previousMillis = 0;     
const long iteration_time = 200;  

float rate_parameter = 1.0 / 10.0;
int i = 0; int count = 0;
int next = 0;        // next iteration

int index = 0;       // index of next neighbor to connect  

String pair_req = "";
String ack_reply = "";

SoftwareSerial xbee(2, 3); // RX, TX
    
void setup() {
  Serial.begin(9600);
  xbee.begin( 9600 );
  
  pinMode(R, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(G, OUTPUT);    

  state = assign_random(3);
  delay(100);
  Serial.println("State: " + state);
  if(state == 0){
       digitalWrite (R, HIGH);
       Serial.println("Red High");
  }else if (state == 1){
       digitalWrite (B, HIGH);
       Serial.println("Blue High");         
  }else if (state == 2){
       digitalWrite (G, HIGH);
       Serial.println("Green High");       
    }

  initialize();

  Serial.println("Xbee ready OK");
  next  = nextTime(rate_parameter);
  //String my_id = get_my_id();
}


void loop() {  // MAIN OUTER LOOP

///////////////////////////////////////////////////////////////
//  THIS PART ACTIVATES NODES ACCORDING TO POISSION PROCESS 
///////////////////////////////////////////////////////////////
if ((count == next)&& (active_mode==false)){
    index = get_neighbor(n);
    TX_dest_addr = neighbors[index]; 
    pair_req = "M" + My_ID;
    Serial.println("Sending to:" + TX_dest_addr + " data:" + pair_req);        
    TX(pair_req, TX_dest_addr); // message sending
    active_mode = true;         // gone in active mode
    startTime = millis();       // time start        
}

///////////////////////////////////////////////////////////////
//  THIS PART IS THE GLOBAL RECEIVER FOR ALL KIND OF MESSAGES 
///////////////////////////////////////////////////////////////  
if (xbee.available()) { 
  //Serial.print("Serial1 RCV:");
  while (xbee.available()) {     // If anything comes in Serial2 (pins 0 & 1)
          //Serial.write(xbee.read());   // read it and send it out Serial (USB)
          // get the new byte:
          char inChar = (char)xbee.read();
          // add it to the inputString:
          recvString += inChar;
          // if the incoming character is a newline, set a flag
          // so the main loop can do something about it:
          if (inChar == '%') {
              //Serial.println("GOT >> " + recvString.substring(0,recvString.length()-1));
              procString = recvString.substring(0,recvString.length()-1);
              //Serial.println("len:" + procString.length());
              type = procString.substring(0,1);             
              if (type.startsWith("M"))  //got a pair request message
              {
                 source_addr = procString.substring(1,3);              
                 if (active_mode == false){   // i am available to get paired
                     block = "0";
                     ack_reply = "A" + My_ID + String(state) + block;
                     Serial.println("Sending " + ack_reply);
                     TX(ack_reply, source_addr);
                     
                 }else if (active_mode == true){   //I am busy (active mode) 
                     block = "1";
                     ack_reply = "A" + My_ID + String(state) + block;
                     Serial.println("Sending " + ack_reply);
                     TX(ack_reply, source_addr);
                 }
              }
             
              if (type.startsWith("A"))  //got an Ack message
              {
                rec_addr = procString.substring(1,3);
                rec_data = procString.substring(3,4);
                rec_block = procString.substring(4,5);
                
                if (active_mode){    //not timeout (Active mode)
                    if(rec_addr.startsWith(TX_dest_addr)){  //got ack from last dest TX node
                      if (rec_block.startsWith("0")){    //other node is not busy
                          Serial.print("got msg within time from ");                         
                          Serial.println(rec_addr);
                          active_mode = false;
                          if (SIR){
                                SIR_change_state(state, rec_data.toInt());               
                          }else{
                               state = rec_data.toInt();       //update my state                          
                               change_state(rec_data.toInt());
                          }
                          next  = nextTime(rate_parameter);
                          Serial.println("next poisson " + String(next));
                          count = 0;                          
                                               
                       }else if(rec_block.startsWith("1")){  //other node is busy
                          //try to pair with another neighbour node
                          Serial.println("Node is busy");
                          
                          next  = 1;
                          Serial.println("next poisson " + String(next));
                          count = 0;                            
                       }
                    }else{  //we got ack from last TX node in inActive mode (DISCARD THIS MSG)
                      Serial.println("got ack from last TX node");   
                   }
                 }
                }
              //////////////////////////
           procString = "";   
           recvString = "";    
          }              
     }    
  }

///////////////////////////////////////////////////////////////
//  AFTER POISSON PROCESS TRIGGERS, active_mode = true and TIMER STARTS TO DE-ACTIVATE 
//  THE NODE (active_mode = false) AFTER CERTAIN TIME (time_out_constant)
/////////////////////////////////////////////////////////////// 
if (active_mode == true){   //active mode timer
  currentTime = millis();
  if(currentTime - startTime > time_out_constant){
      Serial.println("Take long time");
      active_mode = false;
      
      next  = nextTime(rate_parameter);
      Serial.println("next poisson " + String(next));
      count = 0;         
   }
}

///////////////////////////////////////////////////////////////
// THIS IS THE ITERATION FOR POISSION PROCESS
// AFTER EVERY iteration_time, do count++
/////////////////////////////////////////////////////////////// 
unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= iteration_time) {
    previousMillis = currentMillis;
    count += 1;
}
   
}  // end of main void loop


void write_xbee(){
  
      delay(AT_delay);
      xbee.println("ATWR");
      //Serial2.println("ATWR");   
      wait_for_ack();
      //Serial.println();
      //Serial.println("------- ATWR finished--------");
    
      delay(AT_delay);
      xbee.println("ATAC");
      //Serial2.println("ATAC");   
      wait_for_ack();
      //Serial.println();
      //Serial.println("------- ATAC finished--------");  
    
      delay(AT_delay);
      xbee.println("ATCN");
      //Serial2.println("ATCN");   
      wait_for_ack();
      //Serial.println();
      //Serial.println("------- ATCN finished--------"); 

}

void set_dest_addr(String addr){
     if (addr.startsWith("CC")){
        delay(AT_delay);
        xbee.println("ATDH0");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        //Serial.println("------- ATDH done 0 "); 
        delay(AT_delay);                
        xbee.println("ATDL0");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        //Serial.println("------- ATDL done 0"); 
    }else if (addr.startsWith("C2")){
        delay(AT_delay);
        xbee.println("ATDH13A200");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        //Serial.println("------- ATDH done 0 "); 
        delay(AT_delay);                
        xbee.println("ATDL414F7FC2");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        //Serial.println("------- ATDL done 0"); 
     }else {
        xbee.println("ATDH13A200");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        //Serial.println("------- ATDH done 13A200"); 
        delay(AT_delay);                
        xbee.println("ATDL41630C" + addr);
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        //Serial.println("------- ATDL done " + addr);      
     }

}

void TX(String msg, String addr){
     initialize();
     if (addr != "default") {
         set_dest_addr(addr);     
     }
     write_xbee(); 
     xbee.print(msg + "%");
}

void wait_for_ack(){  
  delay(50);
  if (xbee.available()){
      //Serial.print("Xbee reply:");
      while (xbee.available()) {     // If anything comes in Serial2 (pins 0 & 1)
             //Serial.write(xbee.read());   // read it and send it out Serial (USB)
             //flashLed(13, 2, 50);  
             (char)xbee.read();
      }
      //Serial.println();
  }
}

void initialize(){
  xbee.print("+++");
  delay(1000);  
  wait_for_ack();
} 

int assign_random(int n){
  randomSeed(analogRead(0));
  int r=random(100);
  int ret = r%n;
  return ret;
  
}

void change_state(int data){
  //Serial.println("data: " + String(data));
  for (int i = 0; i < 3; i++){
    if (data != i){
      digitalWrite(i+6, LOW);
      //Serial.println(String(i+6) + " LOW");
    }else{
      digitalWrite(i+6, HIGH); 
      //Serial.println(String(i+6) + " HIGH");
    }
  }
    
}

int nextTime(float rate_param){
  //  return (int) -logf(1.0f - (float) random()/ RAND_MAX) / rateParameter;
  int r =  rand();
  float result = -logf(1.0f - (float) r / RAND_MAX) / rate_param;
  while ((int)result == 0){
     r =  rand();
     result = -logf(1.0f - (float) r / RAND_MAX) / rate_param;     
  }
  return (int) result;

}

void SIR_change_state(int my_state, int rcv_state){
     ////// SIR Model//////////////////////////////////////////////////
    if ((rcv_state == 0) && (my_state == 2))                  //0=red, 1=blue and 2=green
        {
         state = rec_data.toInt();  //update my state                          
         change_state(state);
        }
    else if(my_state == 0)
        {
         state = 1;  //update my state to blue (recovered)                          
         change_state(state);
         } 
     //////////////// END of SIR MODEL ///////////////////////////////////////// 
}

int get_neighbor(int n) {
  int randomnumber;
  randomnumber = rand() % n;
  //printf("%dchoosen neighbor\n", randomnumber);
  return randomnumber;
}

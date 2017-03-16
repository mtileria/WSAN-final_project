/*code working properly but printing taking long time after changing status */
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
String data="";
String block = "";
int counter = 0;
boolean start_timer = false;
String My_ID = "34";
String rec_ID = "3C";
long randNumber;
int R = 6;
int B = 7;
int G = 8;
int state = 0;
int time_out_constant = 15000;

unsigned long startTime;
unsigned long currentTime;

SoftwareSerial xbee(2, 3); // RX, TX
 void setup() {
      Serial.begin(9600);
      xbee.begin( 9600 );
      
      pinMode(R, OUTPUT);
      pinMode(B, OUTPUT);
      pinMode(G, OUTPUT);    
    
      state =assign_random(3);
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

  //String my_id = get_my_id();
}
String buffer_TX = "";
String buffer_RX = "";

void loop() { 
///////////////////////////////TEST CODE /////////////
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
       if(inputString.startsWith("send:")){
          int len = inputString.length();
          TX_dest_addr = "3C";
          buffer_TX = "M" + My_ID + "0";
          Serial.println("Sending to:" + TX_dest_addr + " data:" + buffer_TX);    
            
            TX(buffer_TX, TX_dest_addr); // message sending
            start_timer = true;
          // timed_out = false;
            startTime = millis();    // time start     
            block = 1;   //1 means timer is running, 0 means time is timed out!          
       }else{
         Serial.println("sending msg " + inputString);
         TX (inputString, "default");
       }
     inputString = "";    
    }
  }
///////////////////////////////////////////////////////////  
  
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
          //Serial.print(inChar);
          if (inChar == '%') {
              Serial.println("GOT >> " + recvString.substring(0,recvString.length()-1));
              procString = recvString.substring(0,recvString.length()-1);
              type = procString.substring(0,1);              
              if (type == "M")
              {
                   source_addr = procString.substring(1,3);
                   //data = procString.substring(3,4);
                   block = procString.substring(3,4);                 
                   Serial.println("Source is: " + source_addr);
                   //Serial.println("data is: " + data);
                   Serial.println("block is: " + block);
                    buffer_RX = "A" + My_ID + String(state) +"B0";
                       TX(buffer_RX, source_addr);
              // xbee.print("AHi%");
              };
             
              if (type == "A")
              {
               Serial.println("I AM in Ack");
                rec_addr = procString.substring(1,3);
                rec_data = procString.substring(3,4);
                rec_block = procString.substring(4,6);
                Serial.println(rec_addr);
                Serial.println(rec_data);
                Serial.println(rec_block);
                   //not timeout
                    if(rec_addr==TX_dest_addr){  //got ack from last TX node
                      Serial.println("i am inside euals");
                       //other node is not busy
                          Serial.println("got msg within time");
                          Serial.println("Reciever Ack: " + rec_addr);
                          Serial.println("Reciever data: " + rec_data);  
                          Serial.println("Reciever block: " + rec_block);
                          block = "0";
                         change_state( rec_data.toInt());
                          start_timer=false;
                    }//else{  // its not time out and we got ack from last TX node
                    //  Serial.println("got ack from last TX node");   
                  // }
               
                }
              //////////////////////////
           procString = "";   
           recvString = "";    
          }              
     }    
  }

if (start_timer == true){
  currentTime = millis();
  if(currentTime - startTime > time_out_constant){
      //Serial.println("Take long time");
      Serial.println("hello");
      Serial.println(String(startTime));
    //timed_out = true;
      start_timer = false;
   }
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
     if (addr.startsWith("Cord")){
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
             char in = xbee.read();
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
  
  return r%n;
  
}

void change_state(int data){
  for (int i = 0; i < 3; i++){
    if (data != i){
      digitalWrite(i+6, LOW);
      Serial.println(i+6 + "LOW");
    }else{
      digitalWrite(i+6, HIGH); 
      Serial.println(i+6 + "HIGH");
    }
  }
    
}


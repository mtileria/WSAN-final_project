#include <SoftwareSerial.h>

int AT_delay = 100;
int cord = 3;
String inputString = "";
String recvString = "";
int counter = 0;

SoftwareSerial xbee(2, 3); // RX, TX
    
void setup() {
  Serial.begin(9600);
  xbee.begin( 9600 );
  pinMode(13, OUTPUT);
  initialize();
  //String my_id = get_my_id();
}

void loop() { 
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
          String dest_addr = (inputString.substring(5,len));
          TX(inputString, dest_addr);           
       }else{
         Serial.println("sending msg " + inputString);
         TX (inputString, "default");
       }
     inputString = "";    
    }
  }
  
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
              Serial.println("GOT >> " + recvString.substring(0,recvString.length()-1));
           recvString = "";    
          }              
     }
      
  } 
}


void set_PAN_ID(String PAN_ID){
  delay(2000);
  Serial.println("ATID" + PAN_ID);
  //Serial1.println("ATID" + PAN_ID);  
  wait_for_ack(); 
  //Serial.println();     
}

String get_my_id(){
  xbee.println("ATID");
  //Serial1.println("ATID" + PAN_ID);  
  delay(50);
  if (xbee.available()){
      //Serial.print("Xbee1 reply:");
      while (xbee.available()) {     // If anything comes in Serial2 (pins 0 & 1)
             Serial.write(xbee.read());   // read it and send it out Serial (USB)
             //flashLed(13, 2, 50);  
      }
      //Serial.println();
  } 
}

void write_xbee(){
  
      delay(AT_delay);
      xbee.println("ATWR");
      //Serial2.println("ATWR");   
      wait_for_ack();
      //Serial.println();
      Serial.println("------- ATWR finished--------");
    
      delay(AT_delay);
      xbee.println("ATAC");
      //Serial2.println("ATAC");   
      wait_for_ack();
      //Serial.println();
      Serial.println("------- ATAC finished--------");  
    
      delay(AT_delay);
      xbee.println("ATCN");
      //Serial2.println("ATCN");   
      wait_for_ack();
      Serial.println();
      Serial.println("------- ATCN finished--------"); 

}

void set_dest_addr(String addr){
     if (addr.startsWith("Cord")){
        delay(AT_delay);
        xbee.println("ATDH0");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        Serial.println("------- ATDH done 0 "); 
        delay(AT_delay);                
        xbee.println("ATDL0");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        //Serial.println();
        Serial.println("------- ATDL done 0"); 
     }else {
        xbee.println("ATDH13A200");
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        Serial.println();
        Serial.println("------- ATDH done 13A200"); 
        delay(AT_delay);                
        xbee.println("ATDL" + addr);
        //Serial2.println("ATDL" + addr);   
        wait_for_ack();
        Serial.println();
        Serial.println("------- ATDL done " + addr);      
     }

}

void TX(String msg, String addr){
     initialize();
     if (addr != "default") {
         set_dest_addr(addr);     
     }
     write_xbee(); 
     xbee.println(msg + "%");
}

String recv_data(){
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
              Serial.println("GOT >> " + recvString.substring(0,recvString.length()-1));
           recvString = "";    
          }              
     }        
  } 

}

void flashLed(int pin, int times, int wait) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);
    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void wait_for_ack(){  
  delay(50);
  if (xbee.available()){
      Serial.print("Xbee reply:");
      while (xbee.available()) {     // If anything comes in Serial2 (pins 0 & 1)
             Serial.write(xbee.read());   // read it and send it out Serial (USB)
             //flashLed(13, 2, 50);  
      }
      Serial.println();
  }
}

void initialize(){
  //delay(500);
  //Serial.print("Xbee1 sending:");
  xbee.print("+++");
  delay(1000);  
  wait_for_ack();
} 



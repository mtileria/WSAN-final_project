#include <SoftwareSerial.h>

int AT_delay = 100;
int cord = 3;
String inputString = "";
String recvString = "";
String procString = "";
String type = "";
String rec_ack = "";
String rec_data = "";
String rec_block = "";
String source_addr = "";
String data="";
String block = "";
int counter = 0;
String My_ID = "3C";
String rec_ID = "34";

SoftwareSerial xbee(2, 3); // RX, TX
    
void setup() {
  Serial.begin(9600);
  xbee.begin( 9600 );
  pinMode(13, OUTPUT);
  initialize();
  //String my_id = get_my_id();
}
String buffer_TX = "M" + My_ID + "B0";
String buffer_RX = "A" + rec_ID + "B1";
void loop() { 
///////////////////////////////TEST CODE /////////////

//int bytesent = Serial.write()

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
          //String dest_addr = (buffer_TX.substring(0,2));
          String dest_addr = "3C";
         // String dest_addr = ("3CB1%");
          Serial.println("Sending to:" + dest_addr);
          TX(buffer_TX, dest_addr);           
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
              //Serial.print(type);
              //Serial.print(procString);              
              if (type == "M")
              {
                   source_addr = procString.substring(1,3);
                   data = procString.substring(3,4);
                   block = procString.substring(4,5);                 
              Serial.println("Source is: " + source_addr);

              Serial.println("data is: " + data);

              Serial.println("block is: " + block);              
              
              String rec_dest_addr = "34";
              TX(buffer_RX, rec_dest_addr);
              
             // xbee.print("AHi%");
              };
             
              if (type == "A")
              {
               // Serial.println("I AM in");
                rec_ack = procString.substring(1,3);
                rec_data = procString.substring(3,4);
                rec_block = procString.substring(4,5);
                Serial.println("Reciever Ack: " + rec_ack);
                Serial.println("Reciever data: " + rec_data);
                Serial.println("Reciever block: " + rec_block);
                
              }; 
              
              
              
             

              //////////////////////////
           procString = "";   
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



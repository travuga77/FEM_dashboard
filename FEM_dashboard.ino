// CAN Receive Example
//

#include <mcp_can.h>
#include <SPI.h>
#include <TM1638.h>

// define a module o n data pin 5, clock pin 6 and strobe pin 7
TM1638 module(5, 6, 7);

unsigned int ledVal = 0;
unsigned long dispVal = 0;
unsigned int ledDiap;
unsigned long battCurr;
unsigned long time;
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
unsigned int left, right;
char msgString[128];                        // Array to store serial string

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10

unsigned int ReadBytesFrom(byte len, byte beg)
{
  int canMsg;
  for (int i = (beg+len); i>beg; i--)
  {
    canMsg<<=8;
    canMsg+=rxBuf[i-1];
  }
  return canMsg;
}

void setup()
{
  Serial.begin(115200);

  // display a hexadecimal number and set the left 4 dots
  module.setDisplayToHexNumber(0x1234ABCD, 0xF0);
  
  // Initialize MCP2515 running at 8MHz with a baudrate of 250kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Receive Example...");
}

byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void loop()
{

//byte keys = module.getButtons();
  // light the first 4 red LEDs and the last 4 green LEDs as the buttons are pressed
 



  if (rxId==0x120) {
    left=ReadBytesFrom(2,2);
    right=ReadBytesFrom(2,0);
    dispVal=left;
    dispVal*=10000;
    dispVal+=right;
    module.setDisplayToDecNumber(dispVal, 0x00, true);
    dispVal=0;
    rxId=0;
  }

  if (rxId==0x4A0) {
    battCurr=ReadBytesFrom(2,0)/10;
    Serial.print(battCurr);
    Serial.print(" ");
    ledDiap=map(battCurr,0, 250, 0 ,8);
    constrain(ledDiap, 0, 8);
    ledVal=1;
    for (int i = 0; i < ledDiap; ++i) {ledVal <<= 1;}
    Serial.println(ledVal);
    module.setLEDs(ledVal-1);
    rxId=0;
  }
  //if ((millis()-time)>=100) {
  //delay(20);

  //module.clearDisplayDigit(4,0);
  //time=millis();
  //} 

/*
  
    // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
    byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, data);
    if(sndStat == CAN_OK){
      Serial.println("Message Sent Successfully!");
    } else {
      Serial.println("Error Sending Message...");
    }
    //time=millis();
  */
  
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    //if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
    //  sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    //else
    // sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    //Serial.print(msgString);
  
    //if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
    //  sprintf(msgString, " REMOTE REQUEST FRAME");
    //  Serial.print(msgString);
    //} else {
    //  for(byte i = 0; i<len; i++){
    //    sprintf(msgString, " 0x%.2X", rxBuf[i]);
    //    Serial.print(msgString);
    //  }
    //}
        
    //Serial.println();
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

//FEM dashboard prog

#include <mcp_can.h>
#include <SPI.h>
#include <TM1638.h>

// define a module o n data pin 5, clock pin 6 and strobe pin 7
TM1638 module(5, 6, 7);

unsigned long dispVal = 0;
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

  // Initialize MCP2515 running at 8MHz with a baudrate of 250kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  module.setDisplayToDecNumber(0,0,true);
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Receive Example...");
}

byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void loop() {
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    if (rxId==0x120) {
      left=ReadBytesFrom(2,2);
      right=ReadBytesFrom(2,0);
      dispVal=left;
      dispVal*=10000;
      dispVal+=right;
      module.setDisplayToDecNumber(dispVal, 0x00, true);
      data[0]=module.getButtons();
      // send data:  ID = 0x530, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
      byte sndStat = CAN0.sendMsgBuf(0x330, 0, 1, data);
      if(sndStat == CAN_OK){
        Serial.println("Message Sent Successfully!");
      } else {
        Serial.println("Error Sending Message...");
      }
    }


  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

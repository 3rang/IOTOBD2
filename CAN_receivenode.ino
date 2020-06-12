
/* This is sample code for receive valid - CAN frames */
/* CAN 11-bit and 29-bit */

#include <CayenneMQTTESP8266.h>

#include <mcp_can.h>
#include <SPI.h>

#define VIRTUAL_CHANNEL 0
#define VIRTUAL_CHANNELIN 1
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
long unsigned int rpm=0;

char msgString[128];                        // Array to store serial string

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(15);                               // Set CS to pin 10

char ssid[] ="Rapatel";
char passwd[] = "rameshbhai123";

char username[] = "2b81ed60-95f9-11e7-a9f6-4b991f8cbdfd";
char mqtt_password[] = "fdc47a16f83f7a74ac6d6ccee1217b499c7c2822";
char clientID[] = "3d92c1d0-9c56-11ea-93bf-d33a96695544";

byte data[] = {0x02, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{

  Cayenne.begin(username, mqtt_password, clientID,ssid,passwd);

  Serial.begin(115200);
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Receive Example...");
}

void loop()
{
    Cayenne.loop();


  


  if(digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
    //  sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);

        if(rxId == 0x7E9)
        {
          Serial.println("HERERERERERERE..RPMRPMRPMR");
          rpm = ((rxBuf[3] * 256)+ rxBuf[4])/4;
          Serial.println(rpm);
        }
        else
        {
          rpm = 0;
        }
      }
    }
        
    Serial.println();
  }
}
CAYENNE_IN(VIRTUAL_CHANNELIN)
{
  byte sndStat = CAN0.sendMsgBuf(0x7DF, 8, data);
    
    if(sndStat == CAN_OK)
      Serial.println("Message Sent Successfully!");
    else
      Serial.println("Error Sending Message...");
}
CAYENNE_OUT(VIRTUAL_CHANNEL)
{
 //CAYENNE_LOG("Send data for Virtual Channel %d", VIRTUAL_CHANNEL);
  // This command writes the device's uptime in seconds to the Virtual Channel.
  //Cayenne.virtualWrite(VIRTUAL_CHANNEL,rpm);

  CAYENNE_LOG("Send data for Virtual Channel %d", VIRTUAL_CHANNEL);
  // This command writes the device's uptime in seconds to the Virtual Channel.
  Cayenne.virtualWrite(VIRTUAL_CHANNEL,rpm);
  }

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

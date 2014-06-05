#include <Wire.h>
#include <XBee.h>
#define RTC_Address   0x32  //RTC_Address 
#define ARRLEN        8     //Array message lengths

// Create an XBee object at the top of your sketch
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

//Pin Connection 
int Ain = A0;
unsigned char date[7];
boolean ship = false;
int message_pointer = 0;
int status_flag = 0;  //0 = default state, 1 = a message was sent awaiting acknowledgement
int incomingByte = 0;
boolean lc = true;
int state_change = 0;  //0 = no state change, 1 means that a ship recently left/arrived
int msgCount = 0;


// Create arrays for holding data for sending/receiving.
uint8_t messageToBase[ARRLEN];
uint8_t receivedMessage[ARRLEN];

// Specify the address of the remote XBee (this is the SH + SL)
XBeeAddress64 addr64 = XBeeAddress64(0x13A200, 0x408BD0F8);

// Create a TX Request
ZBTxRequest message = ZBTxRequest(addr64, messageToBase, sizeof(messageToBase));

// Create a RX Response
ZBRxResponse rx = ZBRxResponse();

void setup()
{
  resetArray(messageToBase);
  resetArray(receivedMessage);
  Wire.begin();
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial2.begin(9600); 
  I2CWriteDate();//Write the Real-time Clock
  xbee.setSerial(Serial3);
  xbee.begin(Serial3);
  delay(100);
}

void loop()
{
  /* Try to read a packet off serial */
    if (Serial2.available() > 0) {
	  delay(1000); //give time for xbee to receive all of the message
      incomingByte = Serial2.read() - 48;
      if (incomingByte == 72 || incomingByte == 65) {
	    msgCount = 1;
		while (msgCount < 8) {
		
		
		
			Serial.println("\nData read from Rx:");
			receivedMessage[0] = incomingByte;
			Serial.print(receivedMessage[0]);
			Serial.print(" ");
			
			//Serial.print("Number of Bytes to read: ");
			//Serial.println(Serial2.available(), DEC);
			for (int i = 1; i < ARRLEN; i++) {
			  Serial.print(" ");
			  incomingByte = Serial2.read() - 48;
			  receivedMessage[i] = incomingByte;
			  Serial.print(receivedMessage[i]);
			  Serial.print(" ");
			  //receivedMessage[i] = receivedMessage[i];
			}
			I2CupdateClock();
		  }	
		}
    
        /* Decode and consume the message */
        uint8_t type = receivedMessage[0];
        switch (type) {
          case 0x48:  // 'H' heart beat check
            //send reply back to Acknowledge heart beat
            messageToBase[0] = 'A';
            pushDateToMessage();
            xbee.send(message);
            Serial.print("\nA Heartbeat was received, sent back the following reply: ");
            for (int j = 0; j < ARRLEN; j++) {
              Serial.print(messageToBase[j], HEX);
              Serial.print(" ");
            }
            Serial.println();
            resetArray(receivedMessage);
            resetArray(messageToBase);
            break;
          case 0x41: // 'A' acknowledge was received
            if (status_flag = 1) { //
                Serial.println();
                for (int j = 0; j < ARRLEN; j++) {
                  Serial.print(receivedMessage[j], HEX);
                Serial.print(" ");
                }
              Serial.println(); 
              //I2CupdateClock();
              resetArray(messageToBase);
              resetArray(receivedMessage);
              status_flag = 0;
            }
            break;
          default :
            resetArray(receivedMessage);
            break;
        }
      }
    }
  
  message_pointer = 0;
  if(ship){
    if(analogRead(Ain) > 200){
      messageToBase[message_pointer] = 'N';
      message_pointer++;
      
      Serial.println();
      Serial.println();
      Serial.println("Ship Departure");
      
      I2CReadDate();  //Read the Real-time Clock     
      Data_process();//Process the time data
      pushDateToMessage();
      xbee.send(message);
     
      Serial.print("\nMessage to base after read from clock: ");
      for (int j = 0; j < ARRLEN; j++) {
        Serial.print(messageToBase[j], HEX);
        Serial.print(" ");
      }
      Serial.println();
      status_flag = 1;
      if (status_flag == 2) {
        resetArray(messageToBase);
      }
      ship = false;
      delay(1000);
    }
  } else {
    if(analogRead(Ain) < 200){
      messageToBase[message_pointer] = 'S';
      message_pointer++;
      Serial.println();
      Serial.println();
      Serial.println("Ship Arrival");
      I2CReadDate();        //Read the Real-time Clock     
      Data_process();       //Process the time data
      pushDateToMessage();  //put time information in message array
      xbee.send(message);   //send the message over xbee
      
      Serial.print("\nMessage to base after read from clock: ");
      for (int j = 0; j < ARRLEN; j++) {
        Serial.print(messageToBase[j], HEX);
        Serial.print(" ");
      }
      Serial.println(); 
      status_flag = 1;
      ship = true;
      delay(1000);
    }
  }
}
// END OF MAIN LOOP


/* Populates the message to base array with timing information 
    the message_pointer should already start at index 1 when this method is called */
void pushDateToMessage(void)
{
  message_pointer = 1;
  for (int i = 0; i < 7; i++) {
        if (i == 1) {
          messageToBase[message_pointer] = date[i] & 127 + 0;
        } else {
          messageToBase[message_pointer] = date[i] + 0;
        }
        message_pointer++;
      }
  message_pointer = 0;
}
/* Reads the time information from the received message array
   and writes them to the clock*/
void I2CupdateClock(void)
{		
  WriteTimeOn();

  Wire.beginTransmission(RTC_Address);        
  Wire.write(byte(0));//Set the address for writing       
  Wire.write(0x00 + receivedMessage[1]);//second:00     
  Wire.write(0x00 + receivedMessage[2]);//minute:30
  Wire.write(0x00 + receivedMessage[3] | 0x80);//hour:20:00(24-hour format)(8pm)
  Wire.write(0x00 + receivedMessage[4]);//weekday:Wednesday     	
  Wire.write(0x00 + receivedMessage[5]);//day:16th      
  Wire.write(0x00 + receivedMessage[6]);//month:April    	
  Wire.write(0x00 + receivedMessage[7]);//year:2014      
  Wire.endTransmission();

  Wire.beginTransmission(RTC_Address);      
  Wire.write(0x12);   //Set the address for writing   		
  Wire.write(byte(0));            
  Wire.endTransmission(); 

  WriteTimeOff();      
}
//Helper method to reset array to 0's initially and after a message is consumed
void resetArray(uint8_t array[]) {
  for (int j = 0; j < ARRLEN; j++) {
    array[j] = '0';
  }
}

//Read the Real-time data register of SD2403 
void I2CReadDate(void)
{
  unsigned char n=0;

  Wire.requestFrom(RTC_Address,7); 
  while(Wire.available())
  {  
    date[n++]=Wire.read();
  }
  delayMicroseconds(1);
  Wire.endTransmission();
}

//Write the Real-time data register of SD2403 with initial values
void I2CWriteDate(void)
{		
  WriteTimeOn();

  Wire.beginTransmission(RTC_Address);        
  Wire.write(byte(0));//Set the address for writing       
  Wire.write(0x00);//second:00     
  Wire.write(0x30);//minute:30
  Wire.write(0xA0);//hour:20:00(24-hour format)(8pm)
  Wire.write(0x03);//weekday:Wednesday     	
  Wire.write(0x16);//day:16th      
  Wire.write(0x04);//month:April    	
  Wire.write(0x14);//year:2014      
  Wire.endTransmission();

  Wire.beginTransmission(RTC_Address);      
  Wire.write(0x12);   //Set the address for writing   		
  Wire.write(byte(0));            
  Wire.endTransmission(); 

  WriteTimeOff();      
}

//The program for allowing to write to SD2400
void WriteTimeOn(void)
{		
  Wire.beginTransmission(RTC_Address);       
  Wire.write(0x10);//Set the address for writing as 10H      	
  Wire.write(0x80);//Set WRTC1=1      
  Wire.endTransmission();

  Wire.beginTransmission(RTC_Address);    
  Wire.write(0x0F);//Set the address for writing as OFH     	
  Wire.write(0x84);//Set WRTC2=1,WRTC3=1      
  Wire.endTransmission(); 	
}

//The program for forbidding writing to SD2400
void WriteTimeOff(void)
{		
  Wire.beginTransmission(RTC_Address);   
  Wire.write(0x0F);   //Set the address for writing as OFH      	
  Wire.write(byte(0));//Set WRTC2=0,WRTC3=0      
  Wire.write(byte(0));//Set WRTC1=0  
  Wire.endTransmission(); 
}

//Process the time_data
void Data_process(void)
{
  unsigned char i;

  for(i=0;i<7;i++)
  {
    if(i!=2)
      date[i]=(((date[i]&0xf0)>>4)*10)+(date[i]&0x0f);
    else
    {
      date[2]=(date[2]&0x7f);
      date[2]=(((date[2]&0xf0)>>4)*10)+(date[2]&0x0f);
    }
  }
  // Use the serial monitor to see information being transmitted   
  
  Serial.print("Sec = ");//second
  Serial.print(date[0]);
  Serial.print("   Min = ");//minute
  Serial.print(date[1]);
  Serial.print("   H = ");//hour
  Serial.print(date[2]);
  Serial.print("   W = ");//week
  Serial.print(date[3]);
  Serial.print("   D = ");//day
  Serial.print(date[4]);
  Serial.print("   M = ");//month
  Serial.print(date[5]);
  Serial.print("   Y = ");//year
  Serial.print(date[6]);

  Serial.println();
}

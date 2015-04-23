/*
Original code from:		
GSM Shield for Arduino		
www.open-electronics.org		
this code is based on the example of Arduino Labs.		
*/



#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"


InetGSM inet;


//dust sensor stuff
//SoftwareSerial dustSerial(10,11);  //RX,  TX


unsigned int Pm25 = 0;
unsigned int Pm10 = 0;
//end dust sensor stuff




char msg[50];
int numdata;
char inSerial[50];
int i = 0;
boolean started = false;
long feedId = 65329;
char datastreamId[] = "1";
//int dataToSend25 = 0;
//int dataToSend10 = 0;

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
char writeAPIKey[] = "HF3Y3SP6GU2V9LB2";
const int updateThingSpeakInterval = 60 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)
char sentMsg[50];



void setup()
{
  //Serial connection.
  pinMode(9, OUTPUT);

Serial2.begin(9600);
Serial.begin(9600);
Serial.println("GSM Shield testing.");
 


startupGSM900();
    
    //PM Sensor setup
    //dustSerial.begin(9600);
    Pm25=0;
    Pm10=0;
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.

}

void thingspeakPost() {

  
  
  char itoaBuffer[8];

  char end_c[2];
  end_c[0] = 0x1a;
  end_c[1] = '\0';

// PM25 int to char
  char data25[50];
  String str25;
  str25 = String(Pm25);
  str25.toCharArray(data25, 50);
  
// PM10 int to char  
  char data10[50];
  String str10;
  str10 = String(Pm10);
  str10.toCharArray(data10, 50);


if (inet.connectTCP(thingSpeakAddress, 80)) {

   Serial.println("connected to thingspeak");

    gsm.SimpleWrite("POST /update HTTP/1.1\r\n");
    gsm.SimpleWrite("Host: api.thingspeak.com\r\n");
    gsm.SimpleWrite("Connection: close\r\n");
    gsm.SimpleWrite("X-THINGSPEAKAPIKEY: ");
    gsm.SimpleWrite(writeAPIKey);
    gsm.SimpleWrite("\r\n");
    gsm.SimpleWrite("Content-Type: application/x-www-form-urlencoded\r\n");
    gsm.SimpleWrite("Content-Length: ");
    sprintf(sentMsg, "field1=%s&field2=%s", data25, data10);
    itoa(strlen(sentMsg), itoaBuffer, 10);

    gsm.SimpleWrite(itoaBuffer);

    gsm.SimpleWrite("\r\n\r\n");

    gsm.SimpleWrite(sentMsg);

    gsm.SimpleWrite("\r\n\r\n");

    gsm.SimpleWrite(end_c);

  //  Serial.print("sent message " );
 //   Serial.print(sentMsg);
 //   Serial.print(" ");
 //   Serial.print(itoaBuffer);

  }
  else
  {
Serial.println("failed");
    startupGSM900();
  }
}


void restartGSM900()
{
  digitalWrite(9, LOW);
  delay(1000);
  digitalWrite(9, HIGH);
  delay(3000);
  digitalWrite(9, LOW);
  delay(3000);
}

void startupGSM900() {
  
  
  
  if (gsm.begin(115200)) {
Serial.println("\nstatus=READY");
    started = true;
  }
else Serial.println("\nstatus=IDLE");

  if (started) {
    gsm.SimpleWrite("AT");
    delay(1000);
    gsm.WhileSimpleRead();
    //GPRS attach, put in order APN, username and password.
    //If no needed auth let them blank.

    if (inet.attachGPRS("internet", "", ""))
    Serial.println("status=ATTACHED");
    else Serial.println("status=ERROR");
    delay(1000);


    //Read IP address.
    gsm.SimpleWriteln("AT+CIFSR");



    delay(5000);

    //Read until serial buffer is empty.
    gsm.WhileSimpleRead();

  }
  
}


void ProcessSerialData()
{
  
  
  uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;
 while (Serial2.available() > 0) 
  {  
    // from www.inovafitness.com
    // packet format: AA C0 PM25_Low PM25_High PM10_Low PM10_High 0 0 CRC AB
     mData = Serial2.read();     delay(2);//wait until packet is received
    if(mData == 0xAA)//head1 ok
     {
        mPkt[0] =  mData;
        mData = Serial2.read();
        if(mData == 0xc0)//head2 ok
        {
          mPkt[1] =  mData;
          mCheck = 0;
          for(i=0;i < 6;i++)//data recv and crc calc
          {
             mPkt[i+2] = Serial2.read();
             delay(2);
             mCheck += mPkt[i+2];
          }
          mPkt[8] = Serial2.read();
          delay(1);
	  mPkt[9] = Serial2.read();
          if(mCheck == mPkt[8])//crc ok
          {
           Serial2.flush();
           // dustSerial.write(mPkt,10);

            Pm25 = (uint16_t)mPkt[2] | (uint16_t)(mPkt[3]<<8);
            Pm10 = (uint16_t)mPkt[4] | (uint16_t)(mPkt[5]<<8);
            if(Pm25 > 9999)
             Pm25 = 9999;
            if(Pm10 > 9999)
             Pm10 = 9999;            
            //get one good packet
             return;
          }
        }      
     }
   } 
}


void loop()
{

  ProcessSerialData();

  thingspeakPost();
  Serial.print("PM2.5 is: ");
  Serial.println(Pm25);
 Serial.print("PM10 is: ");
 Serial.println(Pm10);
  delay(60000);
  
  



  //if (dataToSend > 500) {
  //  restartGSM900();
    //dataToSend = 0;
 // }

  //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
  //serialhwread();
  //Read for new byte on NewSoftSerial.
  // serialswread();
};


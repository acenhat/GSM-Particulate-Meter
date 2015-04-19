#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.   SIMCOM SIM900 uses Pin 7 & 8
//To change pins for Poweron & Reset (effects begin()), change the lines in GSM.h

//Original code from:
//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.
//Simple sketch to start a connection as client.

//Updated by Alexander Waite Mitchell
//SDS011 Dust Sensor Reader & Update to Thingspeak server over GSM



InetGSM inet;

char msg[50];
int numdata;
char inSerial[50];
int i = 0;
boolean started = false;
long feedId = 65329;
char datastreamId[] = "1";
int dataToSend = 0;

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
char writeAPIKey[] = "you_KEY_here";
const int updateThingSpeakInterval = 16 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)
char sentMsg[50];



void setup()
{
  //Serial connection.
  pinMode(9, OUTPUT);


  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  startupGSM900();
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.

}

void thingspeakPost() {
  
  char itoaBuffer[8];
  char end_c[2];
  end_c[0] = 0x1a;
  end_c[1] = '\0';
  char data[4];
  String str;

  str = String(dataToSend);
  str.toCharArray(data, 4);


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
    sprintf(sentMsg, "field1=%s", data);
    itoa(strlen(sentMsg), itoaBuffer, 10);
    gsm.SimpleWrite(itoaBuffer);
    gsm.SimpleWrite("\r\n\r\n");
    gsm.SimpleWrite(sentMsg);
    gsm.SimpleWrite("\r\n\r\n");
    gsm.SimpleWrite(end_c);
    
    Serial.print("sent message " );
    Serial.print(sentMsg);
    Serial.print(" ");
    Serial.print(itoaBuffer);

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
  if (gsm.begin(2400)) {
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
  else {
    restartGSM900();
  }
}





void loop()
{
  thingspeakPost();
  delay(15000);
  dataToSend += 100;
};


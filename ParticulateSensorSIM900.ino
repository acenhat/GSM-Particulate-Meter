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
int i=0;
boolean started=false;
int dataToSend=30;

// ThingSpeak Settings

char thingSpeakAddress[] = "api.thingspeak.com";
const int updateThingSpeakInterval = 16 * 1000; 
char apiKey[] = "put_your_api_key_here";


void setup()
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(2400)){
    Serial.println("\nstatus=READY");
    started=true;
  }
  else Serial.println("\nstatus=IDLE");
  
  if(started){
     gsm.SimpleWrite("AT");
     delay(1000);
     gsm.WhileSimpleRead();
    //GPRS attach, put in order APN, username and password.
    //If no needed auth let them blank.
    
    if (inet.attachGPRS("put_your_APN_here", "", ""))
      Serial.println("status=ATTACHED");
    else Serial.println("status=ERROR");
    delay(1000);
    
  
    //Read IP address.
    gsm.SimpleWriteln("AT+CIFSR");
    delay(5000);
    //Read until serial buffer is empty.
    gsm.WhileSimpleRead();
  
    //TCP Client GET, send a GET request to the server and
    //save the reply.
   // numdata=inet.httpPOST("www.cosm.com", 80, "/", msg, 50);
    //Print the results.
    /*Serial.println("\nNumber of data received:");
    Serial.println(numdata);  
    Serial.println("\nData received:"); 
    Serial.println(msg);
   */
   };
  //
}
void updateThingSpeak(String dataToSend)
{
if(inet.connectTCP(thingSpeakAddress, 80))
  {

    gsm.SimpleWrite("POST /update HTTP/1.1\n");
    gsm.SimpleWrite("Host: api.thingspeak.com\n");
    gsm.SimpleWrite("Connection: close\n");
    gsm.SimpleWrite("X-THINGSPEAKAPIKEY: "+APIKey+"\n");
    gsm.SimpleWrite("Content-Type: application/x-www-form-urlencoded\n");
    gsm.SimpleWrite("Host: api.cosm.com\n");
    gsm.SimpleWrite("Content-Length: ");
    gsm.SimpleWrite(dataToSend.length());
    gsm.SimpleWrite("\n\n");
    gsm.SimpleWrite(dataToSend);
  
 lastConnectionTime = millis();
if (client.connected())
{
Serial.println("Connecting to ThingSpeak...");
Serial.println();
failedCounter = 0;
}
else
{
failedCounter++;
Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");
Serial.println();
}
}
else
{
failedCounter++;
Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");
Serial.println();
lastConnectionTime = millis();
}
}


   
  
/*  else{ 
    
    Serial.println("Nonn connesso!!!");
    
  }*/



void loop()
{
  //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
 // serialhwread();
  //Read for new byte on NewSoftSerial.
  // serialswread();
};

void serialhwread(){
  i=0;
  if (Serial.available() > 0){
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }
    
    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END")){
      Serial.println("_");
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWriteln(inSerial);
    }
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWriteln("AT+CSQ");
    }
    //Read last message saved.
    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg);
    }
    else{
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }    
    inSerial[0]='\0';
  }
}

void serialswread(){
  gsm.SimpleRead();
}

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to start a connection as client.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;

char msg[50];
int numdata;
char inSerial[50];
int i=0;
boolean started=false;
char apiKey[] = "B-PXnIsZp_s6T59MNvIyJlqnKR2SAKxVdlhpUmFDalR2ST0g";
long feedId = 65329;
char datastreamId[] = "1";
int dataToSend=30;



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
    
    if (inet.attachGPRS("internet.wind", "", ""))
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

if(inet.connectTCP("api.cosm.com", 80))

//  Serial.println("Connesso a Cosm"); 
  
/* gsm.SimpleWrite("GET ");
  gsm.SimpleWrite("/");
  gsm.SimpleWrite(" HTTP/1.0\nHost: ");
  gsm.SimpleWrite("www.google.it");
  gsm.SimpleWrite("\n");
  gsm.SimpleWrite("User-Agent: Arduino");
  gsm.SimpleWrite("\n\n");
  //inet.dettachGPRS();
 */
 gsm.SimpleWrite("POST /v2/feeds/");
    gsm.SimpleWrite(feedId);
    gsm.SimpleWrite("/datastreams/");
    gsm.SimpleWrite(datastreamId);
    gsm.SimpleWrite(".xml?_method=put");
    gsm.SimpleWrite(".csv HTTP/1.1\n");
    gsm.SimpleWrite("Host: api.cosm.com\n");

    gsm.SimpleWrite("X-ApiKey: ");
    gsm.SimpleWrite(apiKey);
    gsm.SimpleWrite("\n");
    gsm.SimpleWrite("Content-Length: ");

    
    gsm.SimpleWriteln("ignoto");

   
  gsm.SimpleWrite(dataToSend);
  gsm.SimpleWrite("\n\n");
  

   
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

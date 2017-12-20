#include <SoftwareSerial.h>
#include <MicroGear.h>
#include <ESP8266WiFi.h>

#define D0 16  // USER LED Wake
#define ledPin  D0        // the number of the LED pin

const char* ssid     = "PL-Hotspot";
const char* password = "9W5dui95";

#define APPID   "Gr7SmartBathRoom"
#define KEY     "Xv2NCUTKyUymWBt"
#define SECRET  "yVdvbbH0paViPYFvUv56eXBRT"

#define ALIAS   "NodeMCU1"
#define TargetWeb "DigitalOUTPUT_HTML_web"
#define Feed "WaterFlowLabSyn"

SoftwareSerial mySerial(D2,D3);
WiFiClient client;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{

  char strState[msglen];
  for (int i = 0; i < msglen; i++) 
  {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }

  Serial.println();

  String stateStr = String(strState).substring(0, msglen);
  
  if(stateStr == "ON") 
  {
    digitalWrite(ledPin, LOW);
//    microgear.chat(TargetWeb, "ON");
    mySerial.write('n');
  } 
  else if (stateStr == "OFF") 
  {
    digitalWrite(ledPin, HIGH);
//    microgear.chat(TargetWeb, "OFF");
    mySerial.write('f');
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

uint8_t dataIn;
uint8_t dataOut;
String data;
char dataa;
void setup() 
{
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  Serial.begin(115200,SERIAL_8N1);
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  mySerial.begin(115200);

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       Serial.print(".");
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);

    pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin,HIGH); // Turn off LED
}
 void loop() 
{
  //input from UART
    if(mySerial.available()>0){
      data = mySerial.readStringUntil('\n');
     // data = mySerial.read();
   //   data += dataa;
      Serial.println(data);
      microgear.chat(TargetWeb, data);
      microgear.writeFeed(Feed,"{Flow:"+ data +"}");
     /* if( dataa =='\n' ){
        Serial.println(data); 
        microgear.chat(TargetWeb, data);
        data="";
      }*/
    }
   
    
  /*
    //output from node
     if(Serial.available()){
      data = Serial.read();
     // microgear.chat(TargetWeb, data);
      mySerial.println(data);  
    }*/
  if(microgear.connected()) 
  {
    microgear.loop();
   // Serial.println("connect...");
  } 
  else 
  {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(250);

}

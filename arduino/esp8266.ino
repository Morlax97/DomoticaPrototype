#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid     = ""; // WIFI SSID
const char* password = ""; // WIFI PASSWORD
char* action = "action/device/1";
char* estado = "status/device/1";
char* server = ""; // SERVER
unsigned int bcastPort = 3000;
unsigned int udpLocalPort = 2500;
int LEDPin; // 13 = D7
int BotPin;// 14 = D5
int override = 0;
int configured = 0;
char* bcastPacket = "{\"id\":1,\"type\":\"device\"}";
char incomingPacket[255];
char* bcastIP = ""; // BROADCAST O SERVER
StaticJsonDocument<200> jsonDocument;
JsonArray array;
WiFiClient wifiClient;
WiFiUDP Udp;
PubSubClient client(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  if (digitalRead(BotPin)) {
    byte *p = new byte(length);
    memcpy(p,payload,length);
    int c = (int)p[0];
    if (c == 48) {
      digitalWrite(LEDPin, LOW);
      }
    else if (c == 49) {
      digitalWrite(LEDPin, HIGH);
      }
    }
  }

void setup() {
  Serial.begin(115200);
  delay(100);
  WiFi.mode(WIFI_STA);
  pinMode(BotPin, INPUT_PULLUP);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(server, 1883);
  client.setCallback(callback);
  client.connect("ESP8266Client");
  client.subscribe(action);

  Udp.begin(udpLocalPort);

  while (!configured){
     delay(1000);
     Udp.beginPacket(bcastIP, bcastPort);
     Udp.write(bcastPacket);
     Udp.endPacket();
     Serial.printf("Setup packet sent!\n");
     int packetSize = Udp.parsePacket();
     if (packetSize) {
       int len = Udp.read(incomingPacket, 255);
       if (len > 0){
         incomingPacket[len] = 0;
         }
       Serial.printf("UDP packet contents: %s\n", incomingPacket);
       auto error = deserializeJson(jsonDocument, incomingPacket);
       array = jsonDocument.as<JsonArray>();
       if(error) {
         Serial.println("deserializeJson() failed\n");
         return;
         }
       else {
         for(JsonVariant device : array) {
           const int pin = device["pin"];
           const String mode = device["mode"];
           if (mode.equals("OUTPUT")) {
             pinMode(pin, OUTPUT);
             LEDPin = pin;
             }
           if (mode.equals("INPUT_PULLUP")) {
             pinMode(pin, INPUT_PULLUP);
             BotPin = pin;
             }
           }
         configured = 1;  
         }
       }
     } 
  }


void loop() {
  delay(100);
  if(!digitalRead(BotPin)) {
    digitalWrite(LEDPin, HIGH);
    if (!override) {client.publish(estado,"1");}
    override = 1;
    }
  else if(digitalRead(BotPin) && override) {
    digitalWrite(LEDPin, LOW);
    client.publish(estado,"0");
    override = 0;
    }
  client.loop();
  }

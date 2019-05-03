#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid     = "HF-Staff";
const char* password = "S74ffW1F1";
char* action = "action/device/1";
char* estado = "status/device/1";
char* server = "cherencio.henryford.esc.edu.ar";
unsigned int bcastPort = 3000;
unsigned int udpLocalPort = 2500;
int LEDPin = 13; // D7
int BotPin = 14; // D5
int override = 0;
int configured = 0;
char* bcastPacket = "{\"id\":1,\"type\":\"device\"}";
char incomingPacket[255];
char* bcastIP = "10.0.20.22";
StaticJsonDocument<200> jsonDocument;
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
       if(error) {
         Serial.println("deserializeJson() failed\n");
         return;
         }
       else{
         const int pin = jsonDocument["pin"];
         const String mode = jsonDocument["mode"];
         if (mode.equals("OUTPUT")) {
           Serial.printf("Configuring pin as output\n");
           pinMode(pin, OUTPUT);
           configured = 1;
           }
         }
       }
     }
}


void loop() {
  delay(1000);
  if(!digitalRead(BotPin)) {
    digitalWrite(LEDPin, HIGH);
    override = 1;
    }
  else if(digitalRead(BotPin) && override) {
    digitalWrite(LEDPin, LOW);
    override = 0;
    }
  if(digitalRead(LEDPin)) {client.publish(estado,"1");}
  else {client.publish(estado,"0");}
  client.loop();
  }

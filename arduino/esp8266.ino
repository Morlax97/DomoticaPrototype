#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid     = "";
const char* password = "";
char* action = "action/device/1";
char* estado = "status/device/1";
char* server = "";
int LEDPin = 13; // D7
int BotPin = 14; // D5
int override = 0;
WiFiClient wifiClient;
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
  pinMode(LEDPin, OUTPUT);
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

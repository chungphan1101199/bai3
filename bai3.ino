#include "DHT.h"  
#include <ESP8266WiFi.h>
#include "PubSubClient.h"


#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Replace the next variables with your SSID/Password combination
const char* ssid = "AndroidAP";
const char* password = "110119971";

const char* mqtt_server = "192.168.43.39";

WiFiClient espClient;
PubSubClient client(espClient);  

int sensor = A0;
int led = 12;
long lastMsg = 0;
char msg[50];
int value = 0;
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp8266/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup() {
  
 Serial.begin(115200);
 Serial.println("DHTxx test!");
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);

 pinMode(led,OUTPUT);
 pinMode(sensor,INPUT);
 dht.begin();
 
}

void loop() 
{
  
 int value=analogRead(sensor);
 int humidity = dht.readHumidity();
 int temperature = dht.readTemperature();

 Serial.print("Sensor: "); 
 Serial.print(value);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  char tempString[8];
  sprintf(tempString, "%d",temperature);
  Serial.print("  Temperature: ");
  Serial.print(tempString);
  client.publish("esp8266/temperature", tempString);
  
  char humString[8];
  sprintf(humString, "%d", humidity);
  Serial.print("  Humidity: ");
  Serial.println(humString);
  client.publish("esp8266/humidity", humString);
  
  delay(5000);
}

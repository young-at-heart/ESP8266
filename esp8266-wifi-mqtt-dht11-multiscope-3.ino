/*
Basic ESP8266 MQTT 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <EEPROM.h>

// WiFi Parameters & MQTT Server IP Address

SimpleDHT11 dht11;
char ssid[] = "ADV";
char password[] = "Mar2ea87";
char mqtt_server[] = "192.168.1.253";
long lastMsg = 0;
char msg1[5];
char msg2[5];
int value1 = 0;
int value2 = 0;
int OUT_LED1 = 5;
int OUT_LED2 = 4;
int EEPROM_ADDR1 = 0;
int EEPROM_ADDR2 = 1;
int DHTPIN = 16;
byte temp = 0;
byte humid = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
// We start by connecting to a WiFi network
Serial.println("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {  
Serial.print("*");
delay(500);
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}

Serial.println();
// Switch on the LED if an 1 was received
if (strcmp(topic, "inTopic/esp3/gpio5") == 0) {
      Serial.print("Change GPIO5 to ");
      if ((char)payload[0] == '1') {
      EEPROM.write(EEPROM_ADDR1, 1);
      EEPROM.commit(); 
      // Turn the LED on
      } else {
      EEPROM.write(EEPROM_ADDR1, 0);
      EEPROM.commit();
      // Turn the LED off
      }
} 

if (strcmp(topic, "inTopic/esp3/gpio4") == 0) {
      Serial.print("Change GPIO4 to ");
      if ((char)payload[0] == '1') {
      EEPROM.write(EEPROM_ADDR2, 1);
      EEPROM.commit();
      // Turn the LED on
      } else {
      EEPROM.write(EEPROM_ADDR2, 0);
      EEPROM.commit();
      // Turn the LED off
      }
}

}
      
void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection ...");
// Attempt to connect
if (client.connect("esp3")) {
Serial.println("connected");
// Once connected, publish an announcement
client.publish("outTopic/esp3", "Hello !!!");
// And resubscribe
client.subscribe("inTopic/esp3/gpio5");
client.subscribe("inTopic/esp3/gpio4");
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
  
pinMode(OUT_LED1, OUTPUT); // Initialize the BUILTIN_LED pin as an output
pinMode(OUT_LED2, OUTPUT); // Initialize the BUILTIN_LED pin as an output
EEPROM.begin(32);
Serial.begin(115200);
setup_wifi();
client.setServer(mqtt_server, 1883);
client.setCallback(callback);

}

void loop() {
if (WiFi.status() != WL_CONNECTED) {
WiFi.disconnect();
delay(500);  
setup_wifi();  
}
if (!client.connected()) {
reconnect();
}
client.loop();
long now = millis();
if (now - lastMsg > 2000) {
lastMsg = now;
dht11.read(DHTPIN, &temp, &humid, NULL);
value1 = (int)temp;
value2 = (int)humid;
snprintf (msg1, 5, "%d", value1);
snprintf (msg2, 5, "%d", value2);
client.publish("outTopic/esp3/temp", msg1);
client.publish("outTopic/esp3/humid", msg2);
}
digitalWrite(OUT_LED1, EEPROM.read(EEPROM_ADDR1));
Serial.print(EEPROM.read(EEPROM_ADDR1));
digitalWrite(OUT_LED2, EEPROM.read(EEPROM_ADDR2));
Serial.print(EEPROM.read(EEPROM_ADDR2));

}

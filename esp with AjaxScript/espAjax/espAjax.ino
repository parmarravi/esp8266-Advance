#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#include "index.h"


// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

int dac = 0;
int DigitalPin[] = {4, 12, 13};
int DacPin = 15;

void setup() {
  Serial.begin(115200);
  delay(10);
  
pinMode(4,INPUT);
pinMode(12,OUTPUT);
pinMode(13,OUTPUT);
  
  Serial.begin(115200);
    WiFiManager wifiManager;
    wifiManager.setSTAStaticIPConfig(IPAddress(192,168,2,99), IPAddress(192,168,2,1), IPAddress(255,255,255,0));// maually Set ip adress 
    wifiManager.resetSettings();
    wifiManager.autoConnect("AutoConnectAP");
    
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  
  // Start TCP (HTTP) server
  server.begin();
  
  Serial.println("TCP server started");
  
  // Add service to MDNS-SD
 // MDNS.addService("http", "tcp", 80);
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String command1 = client.readStringUntil('/');
  String command = client.readStringUntil('/');
  Serial.println(command1);
  Serial.println(command);

if (command == "digital") {
int pin, value;
pin = client.parseInt();
  Serial.println(pin);
if (client.read() == '/') {
value = client.parseInt();
digitalWrite(pin, value);
}
else {
value = digitalRead(pin);
}
client.print(F("digital,"));
client.print(pin);
client.print(F(","));
client.println(value);
}
else if (command == "dac"){
  int pin, value;
pin = client.parseInt();
if (client.read() == '/') {
value = client.parseInt();
dac = value;
analogWrite(pin, value);
}
else {
value = dac;
}
client.print(F("dac,"));
client.print(pin);
client.print(F(","));
client.println(value);
}
else if (command == "status") {
int pin, value;
client.print(F("status"));

for (int thisPin = 0; thisPin < 3; thisPin++) 
{
pin = DigitalPin[thisPin];
value = digitalRead(pin);
client.print(F("#"));
client.print(pin);
client.print(F("="));
client.print(value);
}
{
pin = DacPin;
value = dac;
client.print(F("#"));
client.print(pin);
client.print(F("="));
client.print(value);
}
{
value = analogRead(A0);
float lux=value;
client.print(F("#A0"));
client.print(F("="));
client.print(lux);
}
client.println("");
}
else {  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += file1;  
  client.flush();


  


  // Send the response to the client
while(s.length()>2000)
{
  String dummy = s.substring(0,2000);
  client.print(dummy);
  s.replace(dummy," ");
}
  
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is destroyed
}
}

#include <SoftwareSerial.h>
#include "DHT.h"
#define DHTPIN 5    // what digital pin we're connected to
int ldr=A0;    // 

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial espDebug(2,3);
int chpd =4;

//configure esp wifi

//configure esp wifi
#define SSID "enter the ssid"
#define PASS "enter the password"

//confiuring iot cloud service 
#define IP "184.106.153.149" // ThingSpeak IP Address: 184.106.153.149
String GET = "GET /update?key=4UMLJFY9L4NTD9HG";  // GET /update?key=[THINGSPEAK_KEY]&field1=[data 1]&field2=[data 2]...

void setup()
{
  // put your setup code here, to run once:
Serial.begin(9600);
espDebug.begin(9600);
dht.begin();

pinMode(chpd,OUTPUT);
digitalWrite(chpd,HIGH);
delay(1000);
Serial.println("AT");
delay(5000);

if(Serial.find("OK"))
  {
   espDebug.println("OK");
   delay(1000);
   connectWiFi();
  }
  else 
  {
    resetESP();
  }
}
void loop() 
{
  //Read amount of light  
  int lightValue =analogRead(ldr);   
  // Read humidity 
  int hum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int tem = dht.readTemperature();

  String temp =String(tem);// turn integer to string
  String light = String(lightValue);// turn integer to string
  String humid=String(hum);// turn integer to string
  ATdata(temp,light,humid);
  delay(3000); 
}

void ATdata( String H, String T , String L)
{
  
  // ESP8266 Client
  String cmd = "AT+CIPSTART=\"TCP\",\"";// Setup TCP connection
  cmd += IP;
  cmd += "\",80";
  espDebug.println(cmd);
  delay(2000);
  if( Serial.find( "Error" ) )
  {
    espDebug.println( "RECEIVED: Error\nExit1" );
    return;
  }
  cmd = GET + "&field1=" + H +"&field2="+ T + "&field3=" + L +"\r\n";
  Serial.print( "AT+CIPSEND=" );
  Serial.println( cmd.length() );
  if(Serial.find( ">" ) )
  {
    espDebug.print(">");
    espDebug.print(cmd);
    Serial.print(cmd);
  }
  else
  {
    espDebug.println( "AT+CIPCLOSE" );//close TCP connection
  }
  if( Serial.find("OK") )
  {
    espDebug.println( "RECEIVED: OK" );
  }
  else
  {
    espDebug.println( "RECEIVED: Error\nExit2" );
  }
}

boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");//WiFi STA mode - if '3' it is both client and AP
  delay(2000);
  
  String cmd="AT+CWJAP=\""; //Connect to Router with AT+CWJAP="SSID","Password"
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  
  Serial.println(cmd);
 // espDebug.println(cmd);
  if(Serial.find("OK"))
  {
  delay(5000);
  cmd = "AT+CIPMUX=0";// Set Single connection
  Serial.println(cmd);
   espDebug.println(Serial.readString());
  Serial.println("AT+CWJAP?");
   espDebug.println(Serial.readString()); 
  }
  else
  {
    resetESP();
  }
}

boolean resetESP()
{
  espDebug.println("ARDUINO: esp reset start");
  digitalWrite(chpd,LOW);
  delay(1000);
  digitalWrite(chpd,HIGH);
  delay(500);
  return true;
}


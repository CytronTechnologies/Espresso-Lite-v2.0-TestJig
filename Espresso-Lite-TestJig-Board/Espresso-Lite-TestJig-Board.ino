/*
  ESP8266 mDNS responder sample

  This is an example of an HTTP server that is accessible
  via http://esp8266.local URL thanks to mDNS responder.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to http://esp8266.local, you should see a response.

 */


#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <WiFiClient.h>

const char* station_ssid = "Cytron-Asus";
const char* station_pass = "f5f4f3f2f1";
String softAP_ssid = "";
const char* softAP_pass = "";
IPAddress station_IP(0, 0, 0, 0);
IPAddress softAP_IP(0, 0, 0, 0);
uint32_t uniqueID;

WiFiServer server(80);

bool syncWithRPi(){
  
}

uint8_t* retrieveID(){
  uniqueID = ESP.getFlashChipId();
  softAP_ssid = "ESPert-" + (String)uniqueID;
  uint8_t bytesInID[3] = {0};
  for (int i = 0; i<3; i++)
    bytesInID[i] = (uniqueID >> (i << 3));
  return bytesInID; 
}

bool hardwareTest(){
  
}

bool checkWiFiConnection(){
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(station_ssid, station_pass);
  long _currentTime = millis();
  int timeout = 20000; //10 s
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(millis() - _currentTime > timeout)
      return false;
  }

  station_IP = WiFi.localIP();
  return true;
  
}

bool clientTest(){
  const char destServer[] = "www.adafruit.com";
  int connectRetry = 3;
  bool connectSuccess = false;
  WiFiClient client;
  while(connectRetry-- && !connectSuccess)
  {
    if (!client.connect(destServer, 80))
      continue;
    else
    {
      client.print("GET /testwifi/index.html HTTP/1.1\r\nHost: ");
      client.print(destServer);
      client.print("\r\nConnection: close\r\n\r\n");
      
      int i=5000;
      while (client.available() <= 0 && i--)
        delay(1);
      
      String s, line="";
      while (client.available()>0)
      {
        s=client.readStringUntil('\n');
        if(s.indexOf('\r')==-1) 
          line+=s;
      }
      Serial.println(line);
      if(line.equals("This is a test of the CC3000 module!If you can read this, its working :)"))
        connectSuccess = true;
      client.stop();
    }   
  }
  return connectSuccess;
}

bool serverTest(){

  server.begin();
  long _currentTime = millis();
  int timeout = 10000; //10 s
  // Check if a client has connected within timeout
  bool doneWithClient = false;
  while(!doneWithClient && (millis()-_currentTime < timeout))
  {
    WiFiClient client = server.available();
    if (!client) {
      continue;
    }
    // Wait for data from client to become available
    while(client.connected() && !client.available()){
      delay(1);
    }
    
    // Read the first line of HTTP request
    String req = client.readStringUntil('\r');
    
    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1) {
      Serial.print("Invalid request: ");
      Serial.println(req);
      continue;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print("Request: ");
    Serial.println(req);
    client.flush();
    
    String s;
    if (req == "/")
    {
      IPAddress ip = WiFi.getMode() == WIFI_STA ? station_IP:softAP_IP;
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      s = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<!DOCTYPE HTML>\n";
      s += "<html>Hello from ";
      s += WiFi.getMode() == WIFI_STA ? "Espresso Lite v2.0":softAP_ssid;
      s += " at ";
      s += ipStr;
      s += "</html>\n\n";
      _currentTime = millis();
    } 

    else if(req.equals("/responseOK"))
    {
      s = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n";
      doneWithClient = true;
    }

    client.print(s); 
  }

  return doneWithClient; 
}

bool softAPTest(){

  WiFi.mode(WIFI_AP);
  WiFi.softAP(softAP_ssid.c_str(), softAP_pass);
  delay(2000);
  softAP_IP = WiFi.softAPIP();
  return serverTest();
  
}

bool readyToQuit(){
  
}

void setup(void)
{  
  delay(2000);
  Serial.begin(115200);
  while(!Serial);
  //flush the gibberish value before starting the test
  while(Serial.available())
    Serial.read();
}

//loop function acts as serialEvent to check incoming commands from RPi2
void loop(void)
{
  //temporary test
  String output = "";
  Serial.println("Retrieving ID...");
  uint8_t* receiveID = retrieveID();
  Serial.println("UniqueID : " + (String)uniqueID);
  for (int i = 0; i < sizeof(receiveID)/sizeof(uint8_t) ;i++){
    Serial.print(receiveID[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println("Checking WiFi connection...");
  Serial.println("WiFi connection is " + String(checkWiFiConnection()? "":"not ") +"successful");
  Serial.println("Checking Client Test...");
  Serial.println("Client test is " + String(clientTest()? "":"not ") +"working");
  Serial.println("Checking Server Test...");
  Serial.println(station_IP);
  Serial.println("Server test is " + String(serverTest()? "":"not ") +"working");
  //Serial.println("Checking SoftAP Test...");
  //Serial.println("SoftAP test is " + String(softAPTest()? "":"not ") +"working");
 
  while(1)
  {
    delay(1);
  }

}



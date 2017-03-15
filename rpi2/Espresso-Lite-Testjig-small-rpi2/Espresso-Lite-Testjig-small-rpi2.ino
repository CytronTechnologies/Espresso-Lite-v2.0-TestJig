#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "SSD1306.h"
#include "images.h"

IPAddress station_IP(0,0,0,0);
//const char* station_ssid = "Cytron-Asus";
const char* station_pass = "f5f4f3f2f1";
uint32_t uniqueID;
//const char* devid = "vDB7FF981EFBF784";
const char* devid = "vA4134F32399DA7F";
const char host[] = "api.pushingbox.com";
const int httpPort = 80;

SSD1306 display(0x3c, SDA, SCL);
String clientText="";
String toSent="";

 bool retrieveID(){
  uniqueID = ESP.getChipId();
  toSent = "ESPert-" + (String)uniqueID;
  if(uniqueID)
    return true;
  else
    return false; 
}

bool uploadID()
{
    WiFiClient client;

    String data="GET /pushingbox?devid=";
    data+=devid;
    data+="&data1="; //GET request query to pushingbox API
    data+=toSent;
    data+=" HTTP/1.1\r\n";
    data+="Host: api.pushingbox.com\r\n";
    data+="Connection: close\r\n\r\n";
    
    int retry = 3;
    while(retry--)
    {
      if(client.connect(host, httpPort))
      {
        client.print(data);
        //5 sec to wait for reply from the server
        int i=5000;
        while (client.available()<=0&&i--)
        {
          delay(1);
          if(i==1) return false;
        }
        // Read all the lines of the reply from server and print them to Serial
        while(client.available())
          client.read();   
        return true;
      }
    }

    return false;
}

bool checkWiFiConnection(){
  
  bool retry = false;
  int timeout = 20000; //20 s
  long _currentTime;
  WiFi.mode(WIFI_STA);
  
  WiFi.begin("Cytron Yes", "cytrontechnologiessdnbhd");
  _currentTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    if(millis() - _currentTime > timeout){
      retry = true;
      break;
    }
  }

  if(retry)
  {
    WiFi.begin("Cytron", station_pass);
    _currentTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      if(millis() - _currentTime > timeout){
        retry = true;
        break;
      }
    }
  }
  
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
      if(client.available())
      {
        client.find("\r\n\r\n"); //skip the 
        while (client.available()>0)
            line += (char)client.read();
      }
      
      //Serial.print(line);
      if(line.indexOf("This is a test of the CC3000 module!")!=-1)
      {
        connectSuccess = true;
      }
      client.stop();
    }   
  }
  return connectSuccess;
}

bool serverTest(long time=5000){

  WiFiServer server(80);
  server.begin();
  long _currentTime = millis();
  long _timeout = time; //10 s
  // Check if a client has connected within timeout
  bool doneWithClient = false;
  while(!doneWithClient && (millis()-_currentTime < _timeout))
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
      //Serial.print("Invalid request: ");
      //Serial.println(req);
      continue;
    }
    req = req.substring(addr_start + 1, addr_end);
    //Serial.print("Request: ");
    //Serial.println(req);
    client.flush();
    
    String s;
    if (req == "/")
    {
      IPAddress ip = station_IP;
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      s = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<!DOCTYPE HTML>\n";
      s += "<html>Hello from Espresso Lite v2.0 at ";
      s += ipStr;
      s += "</html>\n\n";
      _currentTime = millis();
    } 

    else if(req.equals("/espert"))
    {
      s = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n";
      doneWithClient = true;
    }

    client.print(s); 
    client.stop();
  }
  
  //server.close();
  if(WiFi.getMode()!=WIFI_STA)
    WiFi.mode(WIFI_STA);
  return doneWithClient; 
}

bool hardwareTest(){

  //test I/O 4, 5, 12, 14
  
  pinMode(12, OUTPUT);
  pinMode(14, INPUT);
  digitalWrite(12,HIGH);delay(1);
  if (!digitalRead(14)) return false;
  digitalWrite(12,LOW);delay(1);
  if(digitalRead(14)) return false;
  //Serial.println("Pin 12 and 14 pass");
  pinMode(12, INPUT);

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);delay(1);
  //Serial.print("A0: "); Serial.println(analogRead(A0));
  if(analogRead(A0)<900) return false;
  digitalWrite(15, LOW);delay(1);
  //Serial.print("A0: "); Serial.println(analogRead(A0));
  if(analogRead(A0)>15) return false;
  pinMode(15, INPUT);

  return true;
}

bool buttonTest()
{
  pinMode(0, INPUT);
  pinMode(13,INPUT);
  display.clear();
  display.drawString(0,0,"Button Test");
  display.drawString(0,32,"Press GPIO0");
  display.display();
  int timeout = 30000;
  while(digitalRead(0)&&timeout--){
    delay(1);
    if(timeout==1) return false;
  }
  display.clear();
  display.drawString(0,0,"Button Test");
  display.drawString(0,32,"Press GPIO13");
  display.display();
  timeout = 30000;
  while(digitalRead(13)&&timeout--){
    delay(1);
    if(timeout==1) return false;
  }
  return true;
 
}

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  //Serial.println();
  WiFi.disconnect();
  delay(10);
  for(int i=0;i<10;i++)
  {
    display.init();
    delay(1);
  }
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display.display();
  delay(1500);
  display.clear();
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
    String output="";
    
    if(!buttonTest())
    {
      display.clear();
      display.drawString(0,0,"Button Test");
      display.drawString(0,16,"Failed");
      display.display();
      digitalWrite(2 , HIGH);
      while(1) delay(1);  
    }
    display.clear();
    display.drawString(0,0,"Button Test");
    display.drawString(0,16,"Successful");
    display.display();
    delay(1000);
    
    display.clear();
    display.drawString(0, 0, "Checking WiFi");
    display.drawString(0, 16, "connection...");
    display.display();
    
    if(!checkWiFiConnection())
    {
      display.clear();
      display.drawString(0,0,"WiFi Connection");
      display.drawString(0,16,"Failed");
      display.display();
      digitalWrite(2 , HIGH);
      while(1) delay(1); 
    }

 
    display.clear();
    display.drawString(0,0,"WiFi Connection");
    display.drawString(0,16,"Successful");
    display.display();
    delay(1000);
    
    display.clear();
    display.drawString(0, 0, "Checking Client");
    display.drawString(0, 16, "Test...");
    display.display();

    if(!clientTest())
    {
      display.clear();
      display.drawString(0,0,"Client test");
      display.drawString(0,16,"Failed");
      display.display();
      digitalWrite(2 , HIGH);
      while(1) delay(1); 
    }

    display.clear();
    display.drawString(0,0,"Client test");
    display.drawString(0,16,"Successful");
    display.display();
    delay(1000);

//  Serial.println("Checking Server Test...");
//  Serial.println(station_IP);
//  Serial.println("Server test is " + String(serverTest()? "":"not ") +"working");
//  Serial.println();

    display.clear();
    display.drawString(0, 0, "Hardware Test");
    display.display();
    
  if(!hardwareTest())
  {
    display.clear();
      display.drawString(0,0,"Hardware test");
      display.drawString(0,16,"Failed");
      display.display();
      digitalWrite(2 , HIGH);
      while(1) delay(1); 
  }
  display.clear();
      display.drawString(0,0,"Hardware test");
      display.drawString(0,16,"Successful");
      display.display();
      delay(1000); 

  display.clear();
  display.drawString(0,0,"Retrieving ID...");
  display.display();
  
  if(!retrieveID())
  {
    display.clear();
      display.drawString(0,0,"Retrieve ID");
      display.drawString(0,16,"Failed");
      display.display();
      digitalWrite(2 , HIGH);
      while(1) delay(1); 
  }

  display.clear();
  display.drawString(0,0,"Uploading ID...");
  display.display();

  if(!uploadID())
  {
    display.clear();
      display.drawString(0,0,"Upload ID");
      display.drawString(0,16,"Failed");
      display.display();
      digitalWrite(2 , HIGH);
      while(1) delay(1); 
  }

  
  /*display.clear();
      display.drawString(0,0,"Waiting for");
      display.drawString(0,16,"Reset");
      display.display();
      delay(1000); 
      ESP.deepSleep(2);*/
      WiFi.disconnect();
  display.clear();
      display.drawString(0,0,"If LED on,");
      display.drawString(0,16,"Test OK");
      display.drawString(0,32,"wait all & press");
      display.drawString(0,48,"button 2");
      display.display();
      delay(1000); 
      
  while(1)
    delay(1);

}

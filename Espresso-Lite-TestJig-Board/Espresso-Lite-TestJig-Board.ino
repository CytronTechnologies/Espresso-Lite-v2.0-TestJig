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
#include <WiFiClient.h>

const char* station_ssid = "NBCWIFI";
const char* station_pass = "yeansaw660916";
String softAP_ssid = "";
IPAddress station_IP(0, 0, 0, 0);
IPAddress softAP_IP(0, 0, 0, 0);
uint32_t uniqueID;

const String finalResult = "";

enum{
  receiveCorrect = 0x01,
  receiveError = 0x02,
  operationSuccessful = 0x31,
  operationFailure = 0x32,
  timeOut = 0x33,
};

bool retrieveID(uint8_t *buffer){
  uniqueID = ESP.getChipId();
  softAP_ssid = "ESPert-" + (String)uniqueID;

  for (int i = 0; i<4; i++)
    buffer[i] = (uniqueID >> (i << 3)) & 0xff;

  return true; 
}

bool checkWiFiConnection(){
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(station_ssid, station_pass);
  long _currentTime = millis();
  int timeout = 20000; //10 s
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
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
      //Serial.println(line);
      if(line.equals("This is a test of the CC3000 module!If you can read this, its working :)"))
        connectSuccess = true;
      client.stop();
    }   
  }
  return connectSuccess;
}

bool serverTest(long time=20000){

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
      IPAddress ip = WiFi.getMode()==WIFI_STA? station_IP:softAP_IP;
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

bool softAPTest(){

  WiFi.disconnect();
  int retry = 3;
  IPAddress targetIP(192, 168, 4, 1);
  
  while(retry-- && softAP_IP != targetIP)
  {
    WiFi.softAP(softAP_ssid.c_str());
    delay(2000);
    softAP_IP = WiFi.softAPIP();
  }
  if (softAP_IP != targetIP)
    return false;

  return serverTest(120000);
}


bool hardwareTest(){
  return true;
}


bool readyToQuit(){
  return true;
}

void sendResponse(uint8_t cmd_code, uint8_t *data=NULL, size_t size=NULL){

  uint8_t _packet_length = 1;
  if(data != NULL){
    if(size!=NULL)
      _packet_length += size;
    else
      _packet_length += sizeof(data);
      // this might give wrong response about amount of data
      // better specify the size of data at the first time if input 
      // data is an array
  }

  int _packet_size = 5 + _packet_length;
  uint8_t send_data[_packet_size];
  send_data[0] = 0x4e;
  send_data[1] = 0x5c;
  send_data[2] = 0x30;
  send_data[3] = _packet_length;
  send_data[4] = cmd_code;
  
  if(data!=NULL){
    for(int i = 0; i< sizeof(data); i++) 
      send_data[5 + i] = data[i];
  }
    
  uint8_t checksum = 0;
  for(int i = 0;i < 4 + _packet_length; i++)
    checksum += send_data[i];  
  send_data[4 + _packet_length] = checksum;
  Serial.write(send_data, sizeof(send_data));
  
}

void sendResponse(uint8_t cmd_code, uint8_t data){
  sendResponse(cmd_code, &data, 1);
}

void sendResponse(uint8_t cmd_code, String data){
  int _packet_length = 1 + data.length();
  uint8_t checksum = 0;
  checksum += 0x4e;
  checksum += 0x5c;
  checksum += 0x30;
  checksum += _packet_length;
  checksum += cmd_code;
  for(int i = 0;i <data.length(); i++)
    checksum += data[i];
    
  Serial.write(0x4e);
  Serial.write(0x5c);
  Serial.write(0x30);
  Serial.write(_packet_length);
  Serial.write(cmd_code);
  Serial.print(data);
  Serial.write(checksum);
}

void handleIncomingCommand(){

  if(Serial.available())
  {
    if(Serial.read() == 0x4e)
    {
      byte command = 0;
      byte checksum = 0;
      byte cal_checksum = 0;
      int packet_length = 1;
      byte receiveData[10]; //reserve for 10
      
      byte p[4];
      Serial.readBytes(p, sizeof(p));
      if(p[0]!=0x5c) return;
      
      if(p[1]==0x01){ 
        //simple query   
        command = p[3];
        packet_length = p[2];

        //retrieve rest of data if there is any and checksum
        byte q[packet_length];
        Serial.readBytes(q, sizeof(q));

        if(sizeof(q)!=1)
          memcpy(receiveData, q, packet_length-1);
        
        checksum = q[packet_length-1];
        cal_checksum = 0x4e;
        for(int i = 0; i< sizeof(p); i++)
          cal_checksum += p[i];
        for(int i = 0; i< (sizeof(q)-1); i++)
          cal_checksum += q[i];
          
        if(checksum != cal_checksum) return;

        //start handling command:
        if(command == '@'){
            sendResponse(receiveCorrect);
        }
          
        else if(command == 'A'){
            sendResponse(receiveCorrect);
            readyToQuit();
            sendResponse(command, operationSuccessful);
            sendResponse(command, finalResult);
        }
          
        else if(command == 'B'){
            sendResponse(receiveCorrect);
            uint8_t receiveID[4] = {0};
            if(!retrieveID(receiveID)) sendResponse('B', operationFailure);
            else{
              sendResponse(command, operationSuccessful);
              sendResponse(command, receiveID, sizeof(receiveID));
            }    
        }
        
        else if(command == 'C'){
            sendResponse(receiveCorrect);
            uint8_t sendIP[4] = {0};
            if(checkWiFiConnection()){
              sendResponse(command, operationSuccessful);
              sendResponse(command, station_IP.toString());
            }
            else
              sendResponse(command, operationFailure);
        }
          
        else if(command == 'D'){
            sendResponse(receiveCorrect);
            if(clientTest())
              sendResponse(command, operationSuccessful);
            else
              sendResponse(command, operationFailure);
        }
          
        else if(command == 'E'){
            sendResponse(receiveCorrect);
            if(serverTest())
            {
              delay(1000);
              sendResponse(command, operationSuccessful);
            }
            else
            {
              delay(1000);
              sendResponse(command, operationFailure);
            }
            ESP.reset(); //reset here to continue on softAP test
        }
          
        else if(command == 'F'){
            
            sendResponse(receiveCorrect);
            digitalWrite(2, LOW); //troubleshooting
            if(softAPTest())
              sendResponse(command, operationSuccessful);
            else
              sendResponse(command, operationFailure);
            for(int i = 0;i <6; i++)
            {
              digitalWrite(2, LOW);
              delay(200);
              digitalWrite(2, HIGH);//troubleshooting
              delay(200);
            }
        }
          
        else if(command == 'G'){
            sendResponse(receiveCorrect);
            if(hardwareTest())
              sendResponse(command, operationSuccessful);
            else
              sendResponse(command, operationFailure);
        }     
      }

      else if(p[1]==0x02){
        //query for streaming data
      }
      
    }
  }
}

void setup(void)
{  
  Serial.begin(115200);
  while(!Serial);
  WiFi.mode(WIFI_STA);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
//  flush the gibberish value before starting the test
//  while(Serial.available())
//    Serial.read();
}

//loop function acts as serialEvent to check incoming commands from RPi2
void loop(void)
{
  //temporary test
  
//  String output = "";
//  Serial.println("Retrieving ID...");
//  uint8_t receiveID[4] = {0};
//  retrieveID(receiveID);
//  Serial.println("UniqueID : " + (String)uniqueID);
//  for (int i = 0; i < sizeof(receiveID) ;i++){
//    Serial.print(receiveID[i], HEX);
//    Serial.print(' ');
//  }
//  Serial.println("Checking WiFi connection...");
//  Serial.println("WiFi connection is " + String(checkWiFiConnection()? "":"not ") +"successful");
//  Serial.println("Checking Client Test...");
//  Serial.println("Client test is " + String(clientTest()? "":"not ") +"working");
//  Serial.println("Checking Server Test...");
//  Serial.println(station_IP);
//  Serial.println("Server test is " + String(serverTest()? "":"not ") +"working");
//  Serial.println("Checking SoftAP Test...");
//  Serial.println("SoftAP test is " + String(softAPTest()? "":"not ") +"working");
 
//  while(1)
//    delay(1);
  
  //real program
  handleIncomingCommand();

  delay(1);

}



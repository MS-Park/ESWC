//Library Includes
#include <SoftwareSerial.h> 
#include <SparkFunESP8266WiFi.h>

//WIFI network id & password
const char mySSID[] = "iptime";
const char myPSK[] = "12345678";

// HTTP Strings 
const char destServer[] = "168.188.129.190";

const String GreenRequest = "GREEN";
const String RedRequest = "RED";
String prevState;

//VARIABLES
float Ro1 = 108251.0;// this has to be tuned 10K Ohm
float Ro2 = 41763.0;
int sensorPin = 0;  // select the input pin for the sensor




// SETUP
void setup() {
  prevState = "GREEN";
  initializeESP8266(); // initailizeESP8266
  connectESP8266();// connectESP8266() connects to the defined WiFi network.
  displayConnectInfo();// displayConnectInfo prints the Shield's local IP
  GreqeustSever();
  Serial.begin(9600);      // initialize serial communication with computer
}
// LOOP
void loop() { 
  
  int val = analogRead(sensorPin);        // variable to store the value coming from the sensor
  float Vrl = 0.0;
  float Rs = 0.0;
  float ratio1 = 0.0;
  float ratio2 = 0.0;  

  Vrl = val * ( 500 / 1024.0  );      // V
  
  Serial.print ( "Air condition :");
  Serial.println(Vrl);
  
  // 농도가 위험한 상태 (아무것도 없을때는 300이하.)
  // 상태 판별하는 조건이 필요한 상태
 if( Vrl > 100 && prevState.equals("GREEN") ){
   RreqeustSever();
   prevState = "RED";
 }
 // CO2,NH3 농도가 위험하지 않은 상태
  else if( Vrl <= 100 && prevState.equals("RED") ){
   GreqeustSever();
   prevState = "GREEN";
  }
  delay(5000);
}
// WIFI SHEILD 초기화
void initializeESP8266()
{
  int test = esp8266.begin();
  if (test != true)
  {
    Serial.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  Serial.println(F("ESP8266 Shield Present"));
}

// WIFI SHEILD 연결
void connectESP8266()
{
  int retVal = esp8266.getMode();
  if (retVal != ESP8266_MODE_STA)
  { 
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if (retVal < 0)
    {
      Serial.println(F("Error setting mode."));
      errorLoop(retVal);
    }
  }
  Serial.println(F("Mode set to station"));
  retVal = esp8266.status();
  if (retVal <= 0)
  {
    Serial.print(F("Connecting to "));
    Serial.println(mySSID);
    retVal = esp8266.connect(mySSID, myPSK);
    if (retVal < 0)
    {
      Serial.println(F("Error connecting"));
      errorLoop(retVal);
    }
  }
}

void displayConnectInfo()
{
  char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  int retVal = esp8266.getAP(connectedSSID);
  if (retVal > 0)
  {
    Serial.print(F("Connected to: "));
    Serial.println(connectedSSID);
  }
  IPAddress myIP = esp8266.localIP();
  Serial.print(F("My IP: ")); Serial.println(myIP);
}

// 서버에 RED 상태 전달
void RreqeustSever()
{
  ESP8266Client client;

  int retVal = client.connect(destServer, 8080);
  if (retVal <= 0)
  {
    Serial.println(F("Failed to connect to server."));
    return;
  }

  client.print(RedRequest);

  while (client.available())
    Serial.write(client.read()); // read() gets the FIFO char
  
  if (client.connected())
    client.stop(); // stop() closes a TCP connection.
}
//서버에 GREEN 상태 전달
void GreqeustSever()
{
  ESP8266Client client;

  int retVal = client.connect(destServer, 8080);
  if (retVal <= 0)
  {
    Serial.println(F("Failed to connect to server."));
    return;
  }

  client.print(GreenRequest);

  while (client.available())
    Serial.write(client.read()); // read() gets the FIFO char
  
  if (client.connected())
    client.stop(); // stop() closes a TCP connection.
}

// errorLoop prints an error code, then loops forever.
void errorLoop(int error)
{
  Serial.print(F("Error: ")); Serial.println(error);
  Serial.println(F("Looping forever."));
  for (;;)
    ;
}


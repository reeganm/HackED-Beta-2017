#include <ESP8266WiFi.h>


// Hardware Stuff
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DeviceAddress Internal, External;

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


float gettemperature(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(deviceAddress);
  //printAddress(deviceAddress);
  //Serial.print("Temp C: ");
  //Serial.println(tempC);
  return(tempC);
}

//Sets up temperature sensors
void InitTempSensors(void)
{
  //temperature sensor stuff
  // Start up the library
  sensors.begin();

  // locate devices on the bus
  // Serial.print("Found ");
  // Serial.print(sensors.getDeviceCount(), DEC);
  // Serial.println(" devices.");

  // search for devices on the bus and assign based on an index.
  // first sensor is external
   if (!sensors.getAddress(External, 0))
   {
     //Serial.println("Unable to find address for Device 0");
   }
   if (!sensors.getAddress(Internal, 1))
   {
      //Serial.println("Unable to find address for Device 1");
    }
}

//Pin definitions for Level Sensor
//https://tttapa.github.io/ESP8266/Chap04%20-%20Microcontroller.html
//orange
#define level0 4
//purple
#define level1 5
//green
#define level2 12
//blue
#define level3 13
//yellow
#define level4 14


int CheckLevel(void)
{
  int l = 0;
  pinMode(level0,INPUT_PULLUP);
  pinMode(level1,INPUT_PULLUP);
  pinMode(level2,INPUT_PULLUP);
  pinMode(level3,INPUT_PULLUP);
  pinMode(level4,INPUT_PULLUP);

  if(digitalRead(level0)==0)
  {
    l = 1;
    //Serial.println("level 0");
  }
  if(digitalRead(level1)==0)
  {
    l = 2;
  //  Serial.println("level 1");
  }
  if(digitalRead(level2)==0)
  {
    l = 3;
//    Serial.println("level 2");
  }
  if(digitalRead(level3)==0)
  {
    l = 4;
    //Serial.println("level3");
  }
  if(digitalRead(level4)==0)
  {
    l = 5;
    //Serial.println("level4");
  }
  pinMode(level0,INPUT);
  pinMode(level1,INPUT);
  pinMode(level2,INPUT);
  pinMode(level3,INPUT);
  pinMode(level4,INPUT);

  return(l);
}

// End of Hardware Stuff


const char* ssid = "SSID";
const char* password = "password";

const char* host = "34.235.10.162";

void setup()
{
//  Serial.begin(115200);
  delay(100);
//  Serial.println();

  InitTempSensors();

//  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
//  Serial.println(" connected");
}


float CupTemp = 0;
float RoomTemp = 0;
int CupLevel = 0;

unsigned long temp_timer = 0;
unsigned long send_timer = 0;
unsigned long temp_update_time = 1000;
unsigned long send_time_interval = 60000;


void loop()
{

  if(millis()-send_timer > send_time_interval)
  {
    send_timer = millis();

      WiFiClient client;


//  Serial.printf("\n[Connecting to %s ... ", host);
  if (client.connect(host, 80))
  {
//    Serial.println("connected]");

//    Serial.println("[Sending a request]");

      // We now create a URI for the request
    String url = "/trigger/post_tweet/with/key/key_here";
    url += "?value1=";
    url += CupLevel;
    url += "&value2=";
    url += CupTemp;
    url += "&value3=";
    url += RoomTemp;

    client.print(String("GET ") +
                url +
                " HTTP/1.1\r\n" +
                "Host: maker.ifttt.com\r\n" +
                "Connection: close\r\n" +
                "\r\n"
                );

//    Serial.println("[Response:]");
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
//    Serial.println("\n[Disconnected]");
  }
  else
  {
//    Serial.println("connection failed!]");
    client.stop();
  }
}

if(millis()-temp_timer>temp_update_time)
{
    temp_timer = millis();

    CupTemp = gettemperature(Internal)+273.15;
    RoomTemp = gettemperature(External)+273.15;
    CupLevel = CheckLevel()*20;

    // Serial.print("Temperature of Cup: ");
    // Serial.print(CupTemp);
    // Serial.println(" C");
    // Serial.print("Temperature of Room: ");
    // Serial.print(RoomTemp);
    // Serial.println(" C");
    // Serial.print("Liquid Level: Less Than ");
    // Serial.print(CupLevel*20);
    // Serial.println(" %");
}

}

// simple Code for reading information from openweathermap.org 

  //#include <WiFi.h> //Library for ESP32
  #include <ESP8266WiFi.h> //Library for ESP8266
  #include <ArduinoJson.h>
  #include <Wire.h>
  #include <TFT_eSPI.h> // Hardware-specific library
  #include <SPI.h>
  #include "Adafruit_GFX.h"
  #include <NTPClient.h> // Date/Time manager 
  #include <WiFiUdp.h>                          // Library to send and receive UDP messages
  #include <WiFiClient.h>     // driver for WiFi client

  #define FS_NO_GLOBALS
  #include <FS.h>
  
  TFT_eSPI tft = TFT_eSPI();       // Invoke custom library


  #define TFT_GREY        0x5AEB
  #define TFT_BLACK       0x0000      
  #define TFT_NAVY        0x000F
  #define TFT_DARKGREEN   0x03E0      
  #define TFT_DARKCYAN    0x03EF      
  #define TFT_MAROON      0x7800      
  #define TFT_PURPLE      0x780F      
  #define TFT_OLIVE       0x7BE0      
  #define TFT_LIGHTGREY   0xC618      
  #define TFT_DARKGREY    0x7BEF      
  #define TFT_BLUE        0x001F  
  #define TFT_GREEN       0x07E0
  #define TFT_CYAN        0x07FF
  #define TFT_RED         0xF800
  #define TFT_MAGENTA     0xF81F      
  #define TFT_YELLOW      0xFFE0      
  #define TFT_WHITE       0xFFFF    
  #define TFT_ORANGE      0xFDA0      
  #define TFT_GREENYELLOW 0xB7E0  
  #define TFT_PINK        0xFC9F    

  // Don't change!!!
  #define TFT_DC  D3
  #define TFT_CS  D8
  #define TFT_MOSI D7
  #define TFT_SCK D5
  #define TFT_RST D4

  //For ESP32
  //#define TFT_MISO 19
  //#define TFT_MOSI 23
  //#define TFT_SCLK 18
  //#define TFT_CS    15  // Chip select control pin
  //#define TFT_DC    2  // Data Command control pin
  //#define TFT_RST   4  // Reset pin (could connect to RST pin)
  //#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is 

  /*#define ESP32_PARALLEL

  // The ESP32 and TFT the pins used for testing are:
  #define TFT_CS   33  // Chip select control pin (library pulls permanently low
  #define TFT_DC   15  // Data Command control pin - must use a pin in the range 0-31
  #define TFT_RST  32  // Reset pin, toggles on startup

  #define TFT_WR    4  // Write strobe control pin - must use a pin in the range 0-31
  #define TFT_RD    2  // Read strobe control pin

  #define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
  #define TFT_D1   13  // so a single register write sets/clears all bits.
  #define TFT_D2   26  // Pins can be randomly assigned, this does not affect
  #define TFT_D3   25  // TFT screen update performance.
  #define TFT_D4   17
  #define TFT_D5   16
  #define TFT_D6   27
  #define TFT_D7   14*/




  const char* ssid     = "**********************";                 // SSID of local network
  const char* password = "**********************";                    // Password on network
  String APIKEY = "**************************";                                 
  String CityID = "*********";                                 //Your City ID

  
  // @TODO: Make Timezone configurable
  int TimeZone = 1;// GMT +1
  // @TODO: Make utcOffsetInSeconds configurable
  const long utcOffsetInSeconds = 3600;
  char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


  WiFiClient client;
  char servername[]="api.openweathermap.org";              // remote server we will connect to
  String result;

  int  counter = 10;                                      

  boolean   night = false;
  String    timeS = "";
  String    day = "";
  int       weatherID = 0;
  String    idString = "";
  String weatherDescription ="";
  String weatherLocation = "";
  String Country;
  float Temperature;
  float Humidity;
  float Pressure;
  int Sunrise;
  int Sunset;
  float Speed;
  float Temp_min;
  float Temp_max;
  float Visibility;
  float Wind_angle;
  float Feels_like;

const unsigned char wifiBitmap [] PROGMEM = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 31, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 128,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 255, 255, 240, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 15, 255, 255, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 255, 255, 254, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 127, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 255, 255, 255, 255, 128, 0, 0, 0, 0, 0, 0, 0, 1, 255, 255, 255, 255, 192, 0, 0,
 0, 0, 0, 0, 0, 3, 255, 255, 255, 255, 224, 0, 0, 0, 0, 0, 0, 0, 7,
 255, 255, 255, 255, 240, 0, 0, 0, 0, 0, 0, 0, 15, 255, 255, 255, 255, 248, 0, 0,
 0, 0, 0, 0, 0, 15, 255, 255, 255, 255, 252, 0, 0, 0, 0, 0, 0, 7, 255, 255,
 255, 255, 255, 255, 248, 0, 0, 0, 0, 0, 127, 255, 255, 255, 255, 255, 255, 255, 0, 0,
 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 192, 0, 0, 0, 3, 255, 255, 255, 255,
 255, 255, 255, 255, 224, 0, 0, 0, 7, 255, 255, 255, 255, 255, 128, 0, 1, 240, 0, 0, 
 0, 7, 255, 255, 255, 255, 255, 0, 0, 0, 120, 0, 0, 0, 15, 255, 255, 255, 255, 254,
 0, 0, 0, 60, 0, 0, 0, 31, 255, 255, 255, 255, 252, 0, 0, 0, 28, 0, 0, 0,
 31, 255, 255, 255, 255, 248, 0, 0, 0, 30, 0, 0, 0, 63, 255, 255, 255, 31, 240, 0,
 0, 30, 14, 0, 0, 0, 63, 7, 7, 6, 15, 240, 255, 255, 62, 14, 0, 0, 0, 63, 7,
 7, 6, 15, 224, 255, 255, 62, 7, 0, 0, 0, 127, 7, 7, 14, 15, 224, 255, 255, 62, 7, 0, 0, 0, 127, 135, 7, 15, 
 31, 224, 255, 255, 30, 7, 0, 0, 0, 127, 135, 7, 15, 255, 224, 255, 255, 0, 3, 0, 0, 0, 127,
 134, 6, 15, 255, 224, 252, 0, 30, 3, 0, 0, 0, 127, 130, 2, 14, 31, 224, 252, 0,
 62, 3, 0, 0, 0, 127, 130, 2, 30, 31, 224, 252, 0, 62, 3, 0, 0, 0, 127, 194, 34, 30, 31, 224,
 252, 0, 62, 3, 0, 0, 0, 127, 194, 34, 30, 31, 224, 255, 254, 62, 3, 0, 0, 0, 127, 194, 34, 30,
 31, 224, 255, 254, 62, 3, 0, 0, 0, 127, 194, 34, 30, 31, 224, 255, 254, 62, 3, 0, 0, 0, 127,
 192, 32, 62, 31, 224, 255, 254, 62, 3, 0, 0, 0, 127, 224, 32, 62, 31, 224, 255, 254, 62, 3, 0, 0,
 0, 127, 224, 96, 62, 31, 224, 255, 254, 62, 3, 0, 0, 0, 127, 224, 112, 62, 31, 224, 252, 0, 62, 3, 0, 0,
 0, 127, 224, 112, 62, 31, 224, 252, 0, 62, 3, 0, 0, 0, 127, 224, 112, 126, 31, 224,
 252, 0, 62, 3, 0, 0, 0, 127, 240, 112, 126, 31, 192, 252, 0, 62, 7, 0, 0, 0, 127, 240, 240, 126, 31, 192,
 252, 0, 62, 7, 0, 0, 0, 63, 255, 255, 255, 255, 192, 248, 0, 62, 7, 0, 0, 0, 63, 255, 255, 255, 255,
 192, 0, 0, 0, 14, 0, 0, 0, 63, 255, 255, 255, 255, 128, 0, 0, 0, 14, 0, 0, 0, 31, 255, 255, 255, 255,
 128, 0, 0, 0, 30, 0, 0, 0, 31, 255, 255, 255, 255, 0, 0, 0, 0, 28, 0, 0, 0, 15, 255, 255, 255,
 255, 0, 0, 0, 0, 56, 0, 0, 0, 7, 255, 255, 255, 254, 0, 0, 0, 0, 248, 0, 0, 0, 7, 255, 255, 255, 252, 
 0, 0, 0, 1, 240, 0, 0, 0, 1, 255, 255, 255, 255, 255, 255, 255, 255, 224, 0, 0, 0, 0, 255, 255, 255,
 255, 255, 255, 255, 255, 192, 0, 0, 0, 0, 63, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0,
 0, 0, 7, 255, 255, 255, 255, 255, 255, 240, 0, 0, 0, 0, 0, 0, 7, 255, 255, 255, 255, 248, 0, 0, 0,
 0, 0, 0, 0, 3, 255, 255, 255, 255, 240, 0, 0, 0, 0, 0, 0, 0, 3, 255, 255, 255, 255, 224, 0, 0, 0, 0,
 0, 0, 0, 1, 255, 255, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 128, 0, 0, 0, 0, 0,
 0, 0, 0, 63, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 255, 255, 254, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 7, 255, 255, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 255, 255, 224, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 127, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  void setup()
  {
  Serial.begin(115200);
  int cursorPosition=0;
  tft.init();
  tft.fillScreen(TFT_BLACK );
  tft.setRotation(0); //2
  tft.setTextWrap(0);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  SPIFFS.begin();
  listFiles();

  #ifdef FORMAT_SPIFFS
    tft.drawString("Formatting SPIFFS, so wait!", 120, 195); SPIFFS.format();
  #endif

  tft.drawBitmap(70, 120, wifiBitmap, 100, 70, TFT_WHITE);
  drawopenweather();
  delay(5000);
  
  //tft.init();
  tft.fillScreen(TFT_BLACK );
  /*tft.drawRect(0,0,240,320,TFT_GREEN);
  tft.drawRect(1,1,238,318,TFT_GREEN);
  tft.drawLine(2,30,240,30, TFT_GREEN);
  tft.drawLine(2,70,240,70, TFT_GREEN);
  tft.drawLine(2,110,240,110, TFT_GREEN);
  tft.drawLine(2,153,240,153, TFT_GREEN);
  tft.drawLine(110,110,110,152, TFT_GREEN);//
  tft.drawLine(2,180,240,180, TFT_GREEN);
  tft.drawLine(2,207,240,207, TFT_GREEN);
  tft.drawLine(2,270,240,270, TFT_GREEN);*/

  

  
  tft.setCursor(10,10);
  tft.print("Connecting");  
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  
  
             while (WiFi.status() != WL_CONNECTED) 
            {
            {
            delay(1000);
            tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
            tft.setTextSize(2);
            tft.setCursor(180,10);
            tft.print(cursorPosition); 
            tft.print(".");
            cursorPosition++;
            timeClient.begin();
            }

             
  }
  
  tft.setCursor(10,10);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.print("Connected to Wifi!");
  Serial.println("Connected to WIFI!");
  delay(1000);
    }

    /*====================================================================================
  This sketch contains support functions for the ESP6266 SPIFFS filing system

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

// -------------------------------------------------------------------------
// List SPIFFS files in a neat format for ESP8266 or ESP32
// -------------------------------------------------------------------------
void listFiles(void) {
  Serial.println();
  Serial.println("SPIFFS files found:");

#ifdef ESP32
  listDir(SPIFFS, "/", true);
#else
  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    // File path can be 31 characters maximum in SPIFFS
    int spaces = 33 - fileName.length(); // Tabulate nicely
    if (spaces < 1) spaces = 1;
    while (spaces--) Serial.print(" ");
    fs::File f = dir.openFile("r");
    Serial.print(f.size()); Serial.println(" bytes");
    yield();
  }

  Serial.println(line);
#endif
  Serial.println();
  delay(1000);
}

#ifdef ESP32
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  fs::File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 8 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }
}
#endif
//====================================================================================
// =======================================================================================
// L O O P
// =======================================================================================

    void loop()
    
    {
    if(counter == 10)                                 //Get new data every 10 minutes
    {
       
      counter = 0;
      displayGettingData();
      delay(1000);
      getWeatherData();
      delay(1000);
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription, weatherID );
       delay(1000);
      displayConditions(Temperature,Humidity,Pressure, Feels_like, Temp_min , Temp_max);
      delay(1000);
      //displayWindSpeed(Speed , Temp_min , Temp_max, Visibility, Wind_angle);
      //delay(1000);
      
     
{
      tft.drawCircle(220,305,10,TFT_WHITE);
      tft.fillCircle(220,305,8,TFT_GREEN);
      delay(6000);
      tft.fillCircle(220,305,8,TFT_BLACK);

      long rssi = WiFi.RSSI();

      tft.setTextSize(1);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(160,310);
      tft.print (rssi);

      tft.setTextSize(1);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(180,310);
      tft.print ("dbm");
      //delay(3000);
      }
    }
    }

    void getWeatherData()                                //client function to send/receive GET request data.
   {
   if (client.connect(servername, 80))   
          {                                         //starts client connection, checks for connection
          client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
          client.println("Host: api.openweathermap.org");
          client.println("User-Agent: ArduinoWiFi/1.1");
          client.println("Connection: close");
          client.println();
          } 
  else {
         Serial.println("connection failed");        //error message if no client connect
          Serial.println();
       }

  while(client.connected() && !client.available()) 
  delay(1);                                          //waits for data
  while (client.connected() || client.available())    
       {                                             //connected or data available
         char c = client.read();                     //gets byte from ethernet buffer
         result = result+c;
       }

  client.stop();                                      //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);
  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);

  if (!root.success())
  {
    Serial.println("parseObject() failed");
  }

  String location = root["name"];
  String country = root["sys"]["country"];
  float temperature = root["main"]["temp"];
  float humidity = root["main"]["humidity"];
  String weather = root["weather"]["main"];
  String description = root["weather"]["description"];
  float pressure = root["main"]["pressure"];
  float sunrise = root["sys"]["sunrise"];
  float sunset = root["sys"]["sunset"];
  float temp_min = root["main"]["temp_min"];
  float temp_max = root["main"]["temp_max"];
  float speed = root["wind"]["speed"];
  float visibility = root["visibility"];
  float wind_angle = root["wind"]["deg"];
  String idstring = root["weather"]["id"];
  float feels_like = root["main"]["feels_like"];
  idString = idstring;
  weatherDescription = description;
  weatherLocation = location;
  Country = country;
  Temperature = temperature;
  Humidity = humidity;
  Pressure = pressure;
  Sunrise = sunrise;
  Sunset = sunset;
  Speed = speed;
  Temp_min = temp_min;
  Temp_max = temp_max;
  Visibility = visibility;
  Wind_angle = wind_angle;
  Feels_like = feels_like;
  
  weatherID = idString.toInt();
 
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(5,295);
  tft.print("IP:");
  tft.print(WiFi.localIP());
  
  
  delay(1000);
 }

  void displayWeather(String location,String description,int weatherID)
 {
  tft.init();
  //tft.setTextSize(2);
  //tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
  //tft.setCursor(10,270);
  //tft.print(location);
  //tft.print(", ");
  //tft.print(Country);
  tft.setCursor(10,170);
  tft.print(description);

  printWeatherIcon(weatherID);
  
  //tft.setTextSize(2);
  //tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  //tft.setCursor(45,50);
  //tft.print("Description");
  
 

  tft.setTextSize(2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setCursor(60,30);
  //tft.print(CityID);
  tft.print("****************");// Your city name
  delay(1000);

 
 }

  void displayConditions(float Temperature,float Humidity, float Pressure, float feels_like, float Temp_min, float Temp_max )
 {
 tft.init();    
 tft.setTextSize(2);
 tft.setTextColor(TFT_YELLOW, TFT_BLACK);
 tft.setCursor(3,50);//Printing Temperature
 tft.print("T:"); 
 
 tft.setTextSize(2);
 tft.setTextColor(TFT_YELLOW, TFT_BLACK);
 //tft.setCursor(20,133);//Printing Temperature
 tft.print(Temperature);
 tft.print("°C"); 

 tft.setCursor(130,50);  
 tft.setTextSize(2);
 tft.setTextColor(TFT_BLUE, TFT_BLACK);                                      
 tft.print("H:"); //Printing Humidity
 
 //tft.setCursor(145,133);  
 tft.setTextSize(2);
 tft.setTextColor(TFT_BLUE, TFT_BLACK);
 tft.print(Humidity,1);
 tft.print(" %"); 

 tft.setTextSize(2);
 tft.setTextColor(TFT_WHITE, TFT_BLACK);
 tft.setCursor(10,250);                     //Printing Pressure
 tft.print("P: ");
 tft.print(Pressure,1);
 tft.print(" hPa");

 tft.setTextSize(2);
 tft.setTextColor(TFT_CYAN, TFT_BLACK);
 tft.setCursor(10,230);                     //Printing Pressure
 tft.print("Feels like:");
 tft.print(Feels_like);
 tft.print("°C");

 tft.setTextSize(2);
 tft.setCursor(10,210);
 tft.setTextColor(TFT_BLUE, TFT_BLACK); 
 //tft.print("Tmin:");
 tft.print(Temp_min);
 tft.print("°C");

 tft.setTextSize(2);
 tft.setCursor(125,210);
 tft.setTextColor(TFT_RED, TFT_BLACK); 
 //tft.print("Tmax:");
 tft.print(Temp_max);
 tft.print("°C");
 delay(1000);
 }

 void displayGettingData()
 {

  
  
 tft.init();
 tft.setTextSize(1);
 tft.setCursor(5,310);
 tft.setTextColor(TFT_GREEN, TFT_BLACK); 
 tft.print(ssid);

 delay(1000);
 }

 /*void displayWindSpeed(float Speed , float Temp_min, float Temp_max , float Visibility , float Wind_angle)
 {

 tft.init();
 tft.setTextSize(2);
 tft.setCursor(10,187);
 tft.setTextColor(TFT_ORANGE, TFT_BLACK); 
 tft.print("Wind:");
 
 tft.setTextSize(2);
 tft.setCursor(95,187);
 tft.setTextColor(TFT_ORANGE, TFT_BLACK); 
 tft.print(Speed,1);
 
 tft.setTextSize(2);
 tft.setCursor(160,187);
 tft.setTextColor(TFT_ORANGE, TFT_BLACK); 
 tft.print("m/s");*/

 

 /*tft.setTextSize(2);
 tft.setCursor(10,252);
 tft.setTextColor(TFT_MAGENTA, TFT_BLACK); 
 tft.print("Vis:");
 tft.print(Visibility,0);
 tft.print(" meters");

 tft.setTextSize(2);
 tft.setCursor(165,210);
 tft.setTextColor(TFT_ORANGE, TFT_BLACK); 
 tft.print("Angle");
 
 tft.setTextSize(2);
 tft.setCursor(170,231);
 tft.setTextColor(TFT_ORANGE, TFT_BLACK); 
 tft.print(Wind_angle,0);
 tft.print("°");
 
 delay(1000);
 }*/

 void drawBmp(const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
     // Serial.print("Loaded in "); Serial.print(millis() - startTime);
     // Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


      void printWeatherIcon(int id) {
 switch(id) {
  case 800: drawBmp("/summer.bmp",70, 65); break;
  case 801: drawBmp("/cloud.bmp",70, 65); break;
  case 802: drawBmp("/cloud.bmp",70, 65); break;
  case 803: drawBmp("/cloudy.bmp",70, 65); break;
  case 804: drawBmp("/cloudy.bmp",70, 65); break;
  
  case 200: drawBmp("/storm.bmp",70, 65); break;
  case 201: drawBmp("/storm.bmp",70, 65); break;
  case 202: drawBmp("/storm.bmp",70, 65); break;
  case 210: drawBmp("/storm.bmp",70, 65); break;
  case 211: drawBmp("/storm.bmp",70, 65); break;
  case 212: drawBmp("/storm.bmp",70, 65); break;
  case 221: drawBmp("/storm.bmp",70, 65); break;
  case 230: drawBmp("/storm.bmp",70, 65); break;
  case 231: drawBmp("/storm.bmp",70, 65); break;
  case 232: drawBmp("/storm.bmp",70, 65); break;

  case 300: drawBmp("/rain2.bmp",70, 65); break;
  case 301: drawBmp("/rain2.bmp",70, 65); break;
  case 302: drawBmp("/rain2.bmp",70, 65); break;
  case 310: drawBmp("/rain2.bmp",70, 65); break;
  case 311: drawBmp("/rain2.bmp",70, 65); break;
  case 312: drawBmp("/rain2.bmp",70, 65); break;
  case 313: drawBmp("/rain2.bmp",70, 65); break;
  case 314: drawBmp("/rain2.bmp",70, 65); break;
  case 321: drawBmp("/rain2.bmp",70, 65); break;

  case 500: drawBmp("/rain.bmp",70, 65); break;
  case 501: drawBmp("/rain.bmp",70, 65); break;
  case 502: drawBmp("/rain.bmp",70, 65); break;
  case 503: drawBmp("/storm.bmp",70, 65); break;
  case 504: drawBmp("/storm.bmp",70, 65); break;
  case 511: drawBmp("/rain.bmp",70, 65); break;
  case 520: drawBmp("/rain.bmp",70, 65); break;
  case 521: drawBmp("/rain.bmp",70, 65); break;
  case 522: drawBmp("/rain.bmp",70, 65); break;
  case 531: drawBmp("/rain.bmp",70, 65); break;

  case 600: drawBmp("/snow.bmp",70, 65); break;
  case 601: drawBmp("/snow.bmp",70, 65); break;
  case 602: drawBmp("/snow.bmp",70, 65); break;
  case 611: drawBmp("/sleet.bmp",70, 65); break;
  case 612: drawBmp("/sleet.bmp",70, 65); break;
  case 615: drawBmp("/sleet.bmp",70, 65); break;
  case 616: drawBmp("/snow.bmp",70, 65); break;
  case 620: drawBmp("/snow.bmp",70, 65); break;
  case 621: drawBmp("/snow.bmp",70, 65); break;
  case 622: drawBmp("/snow.bmp",70, 65); break;

  case 701: drawBmp("/fog.bmp",70, 65); break;
  case 711: drawBmp("/fog.bmp",70, 65); break;
  case 721: drawBmp("/fog.bmp",70, 65); break;
  case 731: drawBmp("/fog.bmp",70, 65); break;
  case 741: drawBmp("/fog.bmp",70, 65); break;
  case 751: drawBmp("/fog.bmp",70, 65); break;
  case 761: drawBmp("/fog.bmp",70, 65); break;
  case 762: drawBmp("/fog.bmp",70, 65); break;
  case 771: drawBmp("/fog.bmp",70, 65); break;
  case 781: drawBmp("/fog.bmp",70, 65); break;
  default:break; 
  }
}

// =======================================================================================
// To switch between day/night icon
void nightOrDay(String timeS) {
  timeS = timeS.substring(0,2);
  int time = timeS.toInt();
  Serial.print ( "====" );
  Serial.print ( time );
  if(time > 20 ||  time<7) {
 night = true;
 tft.invertDisplay(true);}
 
 else {
 night = false;
 tft.invertDisplay(false);
  }
}

// =======================================================================================
// Clear the screen

void clearScreen() {
    tft.fillScreen(TFT_BLACK);
}

void drawClearWeather(){
  if(night){
    drawBmp("/moon.bmp",70, 65);
  }else{
    drawBmp("/summer.bmp",70, 65);
  }
}

void drawFewClouds(){
  if(night){
    drawBmp("/night.bmp",70, 65);
  }else{
    drawBmp("/cloud.bmp",70, 65);
  }
}

void drawTheSun(){
  drawBmp("/summer.bmp",70, 65);
}

void drawTheFullMoon(){
  drawBmp("/moon.bmp",70, 65);
}

void drawTheMoon(){
  drawBmp("/moon.bmp",70, 65);
  
}

void drawCloud(){
  drawBmp("/cloud_sky.bmp",70, 65);
}

void drawThermometer(){
  drawBmp("/heat.bmp",70, 65);
}

void drawUmidity(){
  drawBmp("/humidity.bmp",70, 65);
}

void drawWifi(){
  drawBmp("/wifi_logo.bmp",5, 60);
}

void drawCloudWithSun(){
  
  drawBmp("/cloud.bmp",70, 65);
}

void drawLightRainWithSunOrMoon(){
  if(night){  
    drawBmp("/moon_rain.bmp",70, 65);
  }else{
    drawBmp("/rain.bmp",70, 65);
  }
}

void drawLightRain(){
  drawBmp("/rain.bmp",70, 65);
}

void drawModerateRain(){
  drawBmp("/rain.bmp",70, 65);
}

void drawHeavyRain(){
   drawBmp("/rain.bmp",70, 65);
}

void drawThunderstorm(){
   drawBmp("/storm.bmp",70, 65);
}

void drawLightSnowfall(){
   drawBmp("/snow.bmp",70, 65);
}

void drawModerateSnowfall(){
   drawBmp("/snow.bmp",70, 65);
}

void drawHeavySnowfall(){
   drawBmp("/snow.bmp",70, 65);
}

void drawCloudSunAndRain(){
   drawBmp("/rain.bmp",70, 65);
}

void drawCloudAndTheMoon(){
   drawBmp("/night.bmp",70, 65);
}

void drawCloudTheMoonAndRain(){
   drawBmp("/moon_rain.bmp",70, 65);
}

void drawWind(){  
  drawBmp("/wind.bmp",70, 65); 
}

void drawFog()  {
   drawBmp("/fog.bmp",70, 65);
}

void clearIcon(){
   drawBmp("/summer.bmp", 70, 65);
}
void drawtemp_min(){
  drawBmp("/heat.bmp", 70, 65);
}
void drawtemp_max(){
  drawBmp("/heat.bmp", 70, 65);
}
void drawvisibility(){
  drawBmp("/mountain.bmp", 70, 65);
}
void drawairpressure(){
  drawBmp("/air_pressure.bmp", 70, 65);
}
void drawwindangle(){
  drawBmp("/weathercock.bmp", 70, 65);
}
void drawclouds(){
  drawBmp("/cloud_sky.bmp", 70, 65);
}
void drawsleet(){
  drawBmp("/sleet.bmp", 70, 65);
  }
void drawopenweather(){
  drawBmp("/openweather_logo.bmp", 7,230);
}

void drawday(){
  if(night){
    drawBmp("/night_moon.bmp",0, 80);
  }else{
    drawBmp("/day.bmp",0, 80);
  }
}

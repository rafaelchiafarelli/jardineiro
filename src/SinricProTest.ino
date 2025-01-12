/*
 * Simple example for how to use multiple SinricPro Switch device:
 * - setup 4 switch devices
 * - handle request using multiple callbacks
 * 
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

/***************************************************
  This is our Bitmap drawing example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Arduino.h>
#include <config.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#endif


#include <HTTPClient.h>
#include <WiFiClient.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"


#include <Relays.h>
#include <gf.h>
#include <UI.hpp>


QueueHandle_t rlQueue[4];
int queueSize = 10;
UI ui;
Relays relays;


void Task1code(void *param);
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP req  uest
String header;
String IP_Address;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

TaskHandle_t Task1;

#define LED 21
 
hw_timer_t *Timer0_Cfg = NULL;
 
void IRAM_ATTR Timer0_ISR()
{
    static int rlCount = 0;
    digitalWrite(LED, !digitalRead(LED));
    relays.handler();
    
    ui.setRelayState(rlCount,relays.getRelayState(rlCount));
    rlCount+=1;
    if(rlCount >= relays.getRelaysInstalled())
    {
      rlCount=0;
    }
    
}

bool onPowerState1(const String &deviceId, bool &state) {
  Serial.printf("Device 0 turned %s\r\n",state?"on":"off");
  relayState rl;
  rl.r  = 0;
  rl.st = state;
  xQueueSend(rlQueue[0], &rl, portMAX_DELAY);
  return true; // request handled properly
}

bool onPowerState2(const String &deviceId, bool &state) {
  Serial.printf("Device 1 turned %s\r\n",state?"on":"off");
  relayState rl;
  rl.r  = 1;
  rl.st = state;
  xQueueSend(rlQueue[1], &rl, portMAX_DELAY);
  return true; // request handled properly
}

bool onPowerState3(const String &deviceId, bool &state) {
  Serial.printf("Device 2 turned %s\r\n",state?"on":"off");
  relayState rl;
  rl.r  = 2;
  rl.st = state;
  xQueueSend(rlQueue[2], &rl, portMAX_DELAY);

  rl.r  = 3;
  rl.st = state;
  xQueueSend(rlQueue[3], &rl, portMAX_DELAY);

  return true; // request handled properly
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP); 
    WiFi.setAutoReconnect(true);
  #elif defined(ESP32)
    WiFi.setSleep(false); 
    WiFi.setAutoReconnect(true);
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// setup function for SinricPro
void setupSinricPro() {

    
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);

  SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
  mySwitch2.onPowerState(onPowerState2);

  SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];
  mySwitch3.onPowerState(onPowerState3);


  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
   
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  for(int i=0;i<relays.getRelaysInstalled();i++)
    rlQueue[i] = xQueueCreate(queueSize, sizeof(relayState));
  
  Serial.begin(BAUD_RATE); 
  
  Serial.printf("\r\n\r\n");
  
  setupWiFi();
  setupSinricPro();

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  IP_Address = WiFi.localIP().toString();
  server.begin();
  //RegisterInServer();

  pinMode(LED, OUTPUT);
  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);
  xTaskCreate(
  //xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1",   /* Name of the task */
      15000,     /* Stack size in words */
      NULL,      /* Task input parameter */
      0,         /* Priority of the task */
      &Task1);    /* Task handle. */

      //0);        /* Core where the task should run */

}

void loop() {
  SinricPro.handle();
  ui.handler();
  relayState rlState[4];
  BaseType_t rlCmd[4];
  for(int i=0;i<relays.getRelaysInstalled();i++)
   {
    rlCmd[i]= xQueueReceive(rlQueue[i], &rlState[i], portTICK_PERIOD_MS * 100);
      if (rlCmd[i] != errQUEUE_EMPTY)
      {
        Serial.printf("rl:%d State: %s\r\n",i,rlState[i].st?"on":"off");
        relays.setState(rlState[i]);
      }
   }
  
}


void Task1code(void *parameter)
{
  
  for (;;)
  {

  WiFiClient client = server.available(); // Listen for incoming clients

  if (client)
  { // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected
      currentTime = millis();
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then

        header += c;
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            Serial.write(header.c_str()); // print it out the serial monitor
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("</head><body><div class=\"container\"><div class=\"row\"><h1>Regador</h1></div>");
            
            client.print("<div class=\"row\"> <ul>");
            for(int i=0; i<RELAYS_INSTALLED ; i++)
            {
              client.print("<li>");
              client.print(relays.Names[i].c_str());              

              client.print("<a class=\"btn btn-primary btn-lg\" href=\"?");
              client.printf("%d",i);
              client.print("&On\" id=\"");
              client.print(relays.Names[i].c_str());
              client.print("\" role=\"button\">On</a>");

              client.print("<a class=\"btn btn-primary btn-lg\" href=\"?");
              client.printf("%d",i);
              client.print("&Off\" id=\"");
              client.print(relays.Names[i].c_str());
              client.print("\" role=\"button\">Off</a>");
              client.print("</li>");

            }
            
            client.println("</ul></div></body></html>");
            // The HTTP response ends with another blank line
            client.println();
            if(header.indexOf("GET /?")>=0)
            {
              int pos1 = header.indexOf("?");
              int pos2 = header.indexOf("&");
              String toggleRelay = header.substring(pos1+1,pos2);
              String state = header.substring(pos2+1,header.length());
              
              Serial.print("relay:");
              Serial.println(toggleRelay);
  
              Serial.print("state:");
              Serial.println(state);
              int i = atoi(toggleRelay.c_str());
              if(i>=0 && i<relays.getRelaysInstalled())
              {
                relayState rl;
                rl.r  = i;
                rl.st = false;
                if(state.startsWith("On"))
                {
                  rl.st = true;
                }
                xQueueSend(rlQueue[i], &rl, portMAX_DELAY);
              }
            }
            // Break out of the while loop

            break;
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  else
  {
    vTaskDelay(150);
  }
  }
}

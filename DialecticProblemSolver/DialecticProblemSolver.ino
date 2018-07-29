/*

  Copyright <2018> <Andreas Spiess>

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.


  Based on:

    Google Calendar Integration ESP8266
    Created by Daniel Willi, 2016

    Based on the WifiClientSecure example by
    Ivan Grokhotkov

    Libraries: 
    ArduinoJSON: Benoit Blanchon's lib. Attention !!! Use Stable version 5, not version 6 !!!
    OLED: Daniel Eichhorn's lib: https://github.com/ThingPulse/esp8266-oled-ssd1306

*/

// from LarryD, Arduino forum
#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...)     //now defines a blank line
#define DPRINTLN(...)   //now defines a blank line
#endif

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SSD1306.h>
#include "HTTPSRedirect.h"
//#include <credentials.h>

#define PIN 0
#define DISPLAYWIDTH 15

//Connection Settings
const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
const int httpsPort = 443;

unsigned long entryQuery, entryPrintStatus, entryInterrupt;
String url;
String question, lastQuestion;

int firstSpace, nextSpace, lastSpace, dispLine;
String rest;

SSD1306 display(0x3c, D2, D1);

#ifdef CREDENTIALS
const char*  ssid = mySSID;
const char* password = myPASSWORD;
const char *GScriptIdRead = GoogleScriptIdReadProblemSolver;
#else
//Network credentials
const char*  ssid = ".....";
const char* password = "....."; //replace with your password
//Google Script ID
const char *GScriptIdRead = "AKfycbxtJW3ekcU1Cf2ihRlAMZNkYA4D1sxIEwJ5bx9uiXsxY5RP1yQi"; 
#endif

StaticJsonBuffer<180> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

//Connect to wifi
void connectToWifi() {
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create TLS connection
  HTTPSRedirect client(httpsPort);

  Serial.print("Connecting to ");
  Serial.println(host);
  bool flag = false;
  int retries = 0;
  while (!client.connect(host, httpsPort)) Serial.print(".");
  Serial.println("Connected to Google");
}

String getQuestion() {
  String question;
  Serial.println("Start Question Request");
  HTTPSRedirect client(httpsPort);
  unsigned long getCalenderEntry = millis();
  while (!client.connected() && millis() < getCalenderEntry + 18000) {
    Serial.print(".");
    client.connect(host, httpsPort);
  }
  Serial.println("Fetching Question");
  //Fetch Google Calendar events
  url = String("/macros/s/") + GScriptIdRead + "/exec";
  yield();
  String jsonData = client.getData(url, host, googleRedirHost);
  jsonData = jsonData.substring(jsonData.indexOf('[') + 1, jsonData.indexOf(']'));
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonData);
  // Serial.println(jsonData);
  if (root.success()) {
    const char* q = root["question"];
    question = String(q);
    //   Serial.print("Question---> ");
    //   Serial.println(question);
  } else {
    Serial.println("parseObject() failed");
    question = "Error";
  }
  return question;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN, INPUT_PULLUP);
  display.init();
  display.setFont(ArialMT_Plain_16);
 // display.flipScreenVertically();
  display.drawString(0, 10, "Internet:");
  display.drawString(0, 26, "Socrates:");
  display.display();
  connectToWifi();

}


void loop() {
  if (digitalRead(PIN) == LOW) {
    do {
      question = getQuestion();
    } while (lastQuestion == question);
    lastQuestion = question;
    entryQuery = millis();
    Serial.println(question);
    displayText(question);
  }
}

void displayText(String dispText) {
  rest = dispText;
  dispLine = 0;
  display.clear();
  while (rest.length() > DISPLAYWIDTH) {
    nextSpace = 0;
    lastSpace = 0;
    Serial.print(" Rest ");
    Serial.println(rest);
    do {
      lastSpace = nextSpace;
      nextSpace = rest.indexOf(" ", lastSpace + 1);
      if (nextSpace >= rest.length()) nextSpace = rest.length();
    }  while (nextSpace < DISPLAYWIDTH && rest.length() > DISPLAYWIDTH);
    display.drawString(0, dispLine, rest.substring(0, lastSpace));
    dispLine = dispLine + 15;
    rest = rest.substring(lastSpace + 1, 999);
  }
  display.drawString(0, dispLine, rest.substring(0, 999));
  Serial.println("Display");
  display.display();
}


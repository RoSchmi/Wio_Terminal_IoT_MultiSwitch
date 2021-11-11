// Wio_Terminal_IoT_MultiSwitch
// Shows how to use Sinric pro services, Phone-App and SinricPro_Generic library to control Seeed Wio Terminal
// using IoT technologies 
// This is a modification of
// -https://www.hackster.io/SeeedStudio/wioterminal-iots-multiswitch-b93fcd
// which is based on
// -https://github.com/khoih-prog/SinricPro_Generic/tree/master/examples/Generic/Switch/WIO_Terminal

#include <Arduino.h>

#include "defines.h"
#include "SinricPro_Generic.h"
#include "SinricProSwitch.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();            // Invoke custom library
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

bool powerState1 = false;
bool powerState2 = false;
bool powerState3 = false;

typedef struct
{
   bool actState = false;
   bool lastState = true;
}ButtonState;

ButtonState buttonState1;
ButtonState buttonState2;
ButtonState buttonState3;

int LED1_PIN = LED_BUILTIN;
int LED2_PIN = LED_BUILTIN;
int LED3_PIN = LED_BUILTIN;

typedef struct 
{      
  // struct for the std::map below
  int relayPIN;
  int flipSwitchPIN;
  int index;
} deviceConfig_t;



// this is the main configuration
// please put in your deviceId, the PIN for Relay and PIN for flipSwitch and an index to address the entry
// this can be up to N devices...depending on how much pin's available on your device ;)
// right now we have 2 devicesIds going to 1 LED and 2 flip switches (Wio Terminal left and middle Button)
std::map<String, deviceConfig_t> devices =
{
  //{deviceId, {relayPIN,  flipSwitchPIN, index}}
  // You have to set the pins correctly. 

  { SWITCH_ID_1, {  (int)LED_BUILTIN,  (int)BUTTON_1, 0}},
  { SWITCH_ID_2, {  (int)LED_BUILTIN, (int)BUTTON_2, 1}} 
};

bool onPowerState1(const String &deviceId, bool &state)
{
  Serial.printf("Device 1 turned %s\r\n", state ? "on" : "off");
  int LED1_PIN = devices[deviceId].relayPIN; // get the relay pin for corresponding device
  digitalWrite(LED1_PIN, state);             // set the new relay state

  powerState1 = state;
  digitalWrite(LED1_PIN, powerState1 ? HIGH : LOW);
  spr.fillSprite(TFT_BLACK);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("WIFI :", 10 , 5);
  spr.setTextColor(TFT_GREEN, TFT_BLACK);
  spr.drawString("connected", 100 , 5);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Device1: ", 10, 65);
  spr.drawString("Device2: ", 10, 105);
  //spr.drawString("Device3: ", 10, 145);
  tft.setTextColor(state ? TFT_YELLOW : TFT_DARKGREY, TFT_BLACK);
  tft.drawString(state ? " turned on" : " turned off", 120 , 70, 4);
  return true; // request handled properly
}

bool onPowerState2(const String &deviceId, bool &state)
{
  Serial.printf("Device 2 turned %s\r\n", state ? "on" : "off");
  int LED2_PIN = devices[deviceId].relayPIN; // get the relay pin for corresponding device
  digitalWrite(LED2_PIN, state);             // set the new relay state

  powerState2 = state;
  digitalWrite(LED2_PIN, powerState2 ? HIGH : LOW);
  spr.fillSprite(TFT_BLACK);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("WIFI :", 10 , 5);
  spr.setTextColor(TFT_GREEN, TFT_BLACK);
  spr.drawString("connected", 100 , 5);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Device1: ", 10, 65);
  spr.drawString("Device2: ", 10, 105);
  //spr.drawString("Device3: ", 10, 145);
  tft.setTextColor(state ? TFT_YELLOW : TFT_DARKGREY, TFT_BLACK);
  tft.drawString(state ? " turned on" : " turned off", 120 , 110, 4);
  return true; // request handled properly
  
}

bool onPowerState(String deviceId, bool &state)
{
  Serial.println( String(deviceId) + String(state ? " on" : " off"));
  
  int relayPIN = devices[deviceId].relayPIN; // get the relay pin for corresponding device
  digitalWrite(relayPIN, state);             // set the new relay state

  switch (devices[deviceId].index)
  {
    case 0:
    {
      onPowerState1(deviceId, state);
    }
    break;
    case 1:
    {
      onPowerState2(deviceId, state);
    }
    break;
    case 2:
    {
      //onPowerState3(deviceId, state);
    }
    break;
    case 3:
    {
      //onPowerState4(deviceId, state);
    }
    break;
    default:
    {}
  }

  return true;
}

void handleButtonPress()
{
  if (digitalRead(WIO_KEY_C) == LOW)
  {    
     buttonState1.lastState = buttonState1.actState;
     buttonState1.actState = true;
     if (buttonState1.actState != buttonState1.lastState)
     {      
        powerState1 = !powerState1;
        spr.fillSprite(TFT_BLACK);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        tft.setTextColor(powerState1 ? TFT_YELLOW : TFT_DARKGREY, TFT_BLACK);    
        tft.drawString(powerState1 ? " turned on" : " turned off", 120 , 70, 4);
        digitalWrite(LED1_PIN, powerState1 ? HIGH : LOW);
        // get Switch device back
        SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
        // send powerstate event      
        mySwitch1.sendPowerStateEvent(powerState1); // send the new powerState to SinricPro server
        Serial.print("Device ");
        Serial.print(mySwitch1.getDeviceId().toString());
        Serial.print(powerState1 ? "turned on" : "turned off");
        Serial.println(" (manually via flashbutton)");
      }
      else
      {
        buttonState1.actState = false;
      }
  }

  if (digitalRead(WIO_KEY_B) == LOW)
  {   
     buttonState2.lastState = buttonState2.actState;
     buttonState2.actState = true;
     if (buttonState2.actState != buttonState2.lastState)
     {    
        powerState2 = !powerState2;
        spr.fillSprite(TFT_BLACK);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        tft.setTextColor(powerState2 ? TFT_YELLOW : TFT_DARKGREY, TFT_BLACK);
        tft.drawString(powerState2 ? " turned on" : " turned off", 120 , 110, 4);
        digitalWrite(LED2_PIN, powerState2 ? HIGH : LOW);
        // get Switch device back
        SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
        // send powerstate event     
        mySwitch2.sendPowerStateEvent(powerState2); // send the new powerState to SinricPro server
        Serial.print("Device ");
        Serial.print(mySwitch2.getDeviceId().toString());
        Serial.print(powerState2 ? "turned on" : "turned off");
        Serial.println(" (manually via flashbutton)");
      }
  }
  else
  {
    buttonState2.actState = false;
  }
}

// setup function for WiFi connection
void setupWiFi()
{
  //Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  Serial.println((char *)"First disconnecting.");
  while (WiFi.status() != WL_DISCONNECTED)
  {
    WiFi.disconnect();
    delay(200); 
  }
  
  delay(500);
  Serial.println((char *)"First disconnecting.");
  char buf[100];
  sprintf(buf, "Connecting to SSID: %s", ssid);
  Serial.println(buf);
  
  if (!ssid || *ssid == 0x00 || strlen(ssid) > 31)
  {
    Serial.println((char *)"Invalid: SSID or PWD, Stop");
    // Stay in endless loop
      while (true)
      {         
        delay(1000);
      }
  }

  Serial.println("\n[Wifi]: Connecting");

  WiFi.begin(ssid, password);

  if (!WiFi.enableSTA(true))
  {
    while (true)
    {
      // Stay in endless loop to reboot through Watchdog
      Serial.println((char *)"Connect failed.");
      delay(1000);
      }
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(250);
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(250);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("\n[WiFi]: IP-Address is ");
  Serial.println(WiFi.localIP());

  spr.fillSprite(TFT_BLACK);
  spr.createSprite(280, 175);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("WIFI :", 10 , 5);
  spr.setTextColor(TFT_GREEN, TFT_BLACK);
  spr.drawString("connected", 100 , 5);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Device1: ", 10, 65);
  spr.drawString("Device2: ", 10, 105);
  //spr.drawString("Device3: ", 10, 145);
  spr.pushSprite(0, 0);

}


void setupSinricPro()
{
  for (auto &device : devices)
  {
    const char *deviceId = device.first.c_str();
    SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];    //temp
    mySwitch1.onPowerState(onPowerState1);

    SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];    //light
    mySwitch2.onPowerState(onPowerState2);

    //SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_1];    //humi
    //mySwitch3.onPowerState(onPowerState3);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}


void setup()
{
  Serial.begin(BAUD_RATE);
  //while (!Serial);

  pinMode(LED1_PIN, OUTPUT); // define LED GPIO as output
  pinMode(LED2_PIN, OUTPUT); // define LED GPIO as output
  pinMode(LED3_PIN, OUTPUT); // define LED GPIO as output
 
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  digitalWrite(LED1_PIN, LOW); // turn off LED on bootup
  digitalWrite(LED2_PIN, LOW); // turn off LED on bootup
  digitalWrite(LED3_PIN, LOW); // turn off LED on bootup

  tft.begin();
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  spr.fillSprite(TFT_BLACK);
  spr.createSprite(280, 175);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("WIFI :", 10 , 5);
  spr.setTextColor(TFT_RED, TFT_BLACK);
  spr.drawString("disconnected", 100 , 5);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Device1: ", 10, 65);
  spr.drawString("Device2: ", 10, 105);
  //spr.drawString("Device3: ", 10, 145);
  spr.pushSprite(0, 0);
  
  //RoSchmi
  Serial.println("Starting");
  char buf[100] {0};
  sprintf(buf, "\r\nRTL8720 Firmware: %s\r\n", rpc_system_version());
  Serial.println(buf);
  setupWiFi();
  setupSinricPro();
}

void loop()
{
  SinricPro.handle();
  delay(500);
  handleButtonPress();
}

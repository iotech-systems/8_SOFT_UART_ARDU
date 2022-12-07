
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <PZEM004Tv30.h>
#include "global.h"
#include "core.h"
#include "button.h"
#include "pezpxy.h"


PezProxy __pezpxy;
enum ActionName CURRENT_ACTION = None; 


void setup() {
   CURRENT_ACTION = Init; 
   Serial.begin(COM_BAUDRATE);
   Serial.print(F("SERIAL_TX_BUFFER_SIZE: "));
   Serial.println(SERIAL_TX_BUFFER_SIZE);
   InitLCD();
   delay(4000);
   __LCD.clear();
   InitSoftUarts();
   delay(2000);
   __LCD.clear();
   /* eeprom stuff */
   EEPROM.begin();
   formatEEPROM();
   /* - - */
   Serial.println(F("setup is done"));
   __pezpxy.InitLCD(1);
   __LCD.setCursor(3, 2);
   __LCD.print(F("setup is done"));
   /* -- */
   __BTN_1.onPushRun(btn1_OnPush);
   __BTN_1.onRunRun(btn1_OnRun);
   __BTN_2.onPushRun(btn2_OnPush);
   __BTN_2.onRunRun(btn2_OnRun);
   /* display config data */
   displayConfigInfo();
   /* let user read it */
   delay(2000);
   __LCD.noBacklight();
};


bool withUpstreamPush = false;
uint8_t lpcnt = 0;


void loop() {   
   /* sends data out to edge device */
   if (isTimeToRunUpstreamPush()) {
      CURRENT_ACTION = ReadPush;
      withUpstreamPush = true;
      runOnAllPezmPorts(withUpstreamPush);
   }
   /* prints to lcd and if error sends to edge device */
   if (isTimeToRunDiag()) {
      CURRENT_ACTION = Diag;
      withUpstreamPush = false;
      runOnAllPezmPorts(withUpstreamPush);
      if (BREAK_PINS_LOOP) {
         BREAK_PINS_LOOP = false;
         return;
      }
   }
   /* sleep & mon buttons */
   CURRENT_ACTION = Sleep;
   __pezpxy.InitLCD(2);
   __pezpxy.MonitorSystem(100, 10);
   /* ping upstream */
   if (lpcnt == 8) {
      Serial.println(F("#RPT|MODE:Sleep!"));
      lpcnt = 0;
   }
   lpcnt++;
};

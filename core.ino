
#include <LiquidCrystal_I2C.h>
#include <PZEM004Tv30.h>
#include "core.h"


#define READ_DELAY            200


void InitLCD() {
   __LCD.init();
   __LCD.backlight();
   /* print iotech.systems name */
   lcdln = 0;
   __LCD.setCursor(0, lcdln);
   __LCD.print(F("iotech.systems"));
   /* print model & version */
   __LCD.setCursor(0, ++lcdln);
   __LCD.print(F("Model: "));
   __LCD.print(model());
   __LCD.setCursor(2, ++lcdln);
   __LCD.print(F("version: "));
   __LCD.print(ver());
   __LCD.setCursor(2, 3);
   __LCD.print(F("s/n: "));
   __LCD.print(serialnumber());
};


void InitSoftUarts() {
   /* - - */
   __LCD.setCursor(0, 0);
   __LCD.print(F("-> InitSoftUarts")); 
   /* - - */
   for (uint8_t i = 0; i < SOFT_UART_CNT; i++) {
      Serial.println(softUartsArr[i].name);
      __LCD.setCursor(1, 1);
      __LCD.print(F("Name: "));
      __LCD.print(softUartsArr[i].name);
      __LCD.print(F("  ID: "));
      __LCD.print(softUartsArr[i].id);
      __LCD.setCursor(4, 2);
      __LCD.print(F("rx_pin: "));
      __LCD.print(softUartsArr[i].rxPin);
      __LCD.setCursor(4, 3);
      __LCD.print(F("tx_pin: "));
      __LCD.print(softUartsArr[i].txPin);
      /* program ids */
      checkUnitAddress(softUartsArr[i]);
      /* 1s */
      __pezpxy.MonitorSystem(10, 100);
   }
   /* - - */
};

void checkUnitAddress(SoftUartPins* pins) {
   /* program ids */
   SoftwareSerial softUart(pins->rxPin, pins->txPin);
   PZEM004Tv30 pzem(softUart);
   uint8_t id = pzem.getAddress();
   if (id != pins->id) {      
      __LCD.clear();
      __LCD.setCursor(0, 0);
      __LCD.print(F("--[ SoftUartErr ]--"));
      /* -- */
      __LCD.setCursor(2, 1);
      __LCD.print(F("Name: "));
      __LCD.print(softUartsArr[i].name);
      /* -- */
      __LCD.setCursor(2, 2);
      __LCD.print(F("BadID: "));
      __LCD.print(id);
      /* -- */
      __LCD.print(F(" : "));
      __LCD.print(softUartsArr[i].id);
      /* -- */
      __LCD.setCursor(4, 3);
      if (pzem.setAddress(softUartsArr[i].id)) {
         __LCD.print(F("SetNewID:OK"));
      } else {
         __LCD.print(F("SetNewID:ERR"));
      }
};


/* {ERR_READ_WATTS, ERR_READ_AMPS, ERR_READ_VOLTS
      , ERR_READ_FREQ, ERR_READ_ENERGY}; */
__FlashStringHelper* ReadErrorMsg(ReadErrors errCode) {
   switch (errCode) {
      case READ_WATTS:
         return F("Watts");
      case READ_AMPS:
         return F("Amps");
      case READ_FREQ:
         return F("Freq");
      case READ_ENERGY:
         return F("kWh");
      case READ_VOLTS:
         return F("Volts");
      default:
         return F("BadReadErrorCode");
   }
};


/*
*/
void runOnAllPezmPorts(bool withPush) {
   /* - - */
   for (uint8_t idx = 0; idx < SOFT_UART_CNT; idx++) {
      __pezpxy.Clear();
      SoftUartPins* sup = &softUartsArr[idx];
      __pezpxy.SetPins(sup);
      SoftwareSerial softUart(sup->rxPin, sup->txPin);
      PZEM004Tv30 pzem(softUart, sup->id);
      __pezpxy.SetPzem(&pzem);
      __pezpxy.InitLCD();
      __pezpxy.Run(withPush);
      /* check buttons & display report on lcd */
      /* 4s */
      __pezpxy.MonitorSystem(40, 100);
      if (BREAK_PINS_LOOP)
         break;
   }
};


/* 
*/
bool isTimeToRunUpstreamPush() {
   /* -- */
   if (LAST_UPSTREAM_REPORT == 0) {
      LAST_UPSTREAM_REPORT = millis();
      return true;
   }
   /* -- */
   if ((millis() - LAST_UPSTREAM_REPORT) < MINUTES_6_INTV)
      return false;
   /* -- */
   LAST_UPSTREAM_REPORT = millis();
   /* why? this will disable dry run right after upstream push run */
   LAST_DIAG_RUN = LAST_UPSTREAM_REPORT;
   return true;
};


/* 
*/
bool isTimeToRunDiag() { 
   if ((millis() - LAST_DIAG_RUN) < MINUTES_2_INTV)
      return false;
   /* -- */
   LAST_DIAG_RUN = millis();
   return true;
};


/* 
*/
__FlashStringHelper* currentRunMode() {
   switch (CURRENT_ACTION) {
      case Init:
         return F("Init");
      case ReadPush:
         return F("ReadPush");
      case Diag:
         return F("Diag");
      case Sleep:
         return F("Sleep");
      case PushErrorUp:
         return F("PushErrorUp");
      case PushDataUp:
         return F("PushDataUp");
      default:
         return F("BadRunMode");
   }
};


/*
*/
__FlashStringHelper* ver() {
   return F("0.1");
};


/*
*/
__FlashStringHelper* model() {
   return F("PZEM_READER");
};


/*
*/
char* serialnumber() {
   char c = NULL;
   memset(TMP_BUFF_32, NULL, sizeof(TMP_BUFF_32));
   for (uint8_t i = 0; i < (EEP_DEV_SN_END - EEP_DEV_SN_START); i++) {
      c = EEPROM.read((EEP_DEV_SN_START + i));
      TMP_BUFF_32[i] = c;
   }
   /* -- */
   return TMP_BUFF_32;
};


void btn1_OnPush() {
   Serial.println(F("btn1_OnPush"));
   if (CURRENT_ACTION == ReadPush) {
      Serial.println(F("NoModeInteropt"));
      return;
   }
   /* -- */
   __LCD.backlight();
   if (CURRENT_ACTION == Sleep) 
      __pezpxy.InitLCD(4);
   if (CURRENT_ACTION == Diag) {
      __pezpxy.InitLCD(5);
      /* break for loop */
      BREAK_PINS_LOOP = true;
   }
   /* reset last dts */
   LAST_DIAG_RUN = (LAST_DIAG_RUN - MINUTES_2_INTV);      
   delay(1000);
};


void btn2_OnPush() {
    Serial.println(F("btn2_OnPush"));
};


void btn1_OnRun(Button* pBtn) {
   if ((millis() - pBtn->getLastPushed()) < MINUTES_2_INTV) {
      __LCD.backlight();         
   } else {
      __LCD.noBacklight();
   }
};


void btn2_OnRun(Button* pBtn) {
   static uint8_t runcnt = 0;
   if (++runcnt == 9) {
      Serial.println(F("btn2_OnRun"));
      runcnt = 0;
   }
};


void checkComPort() {
   if (!Serial.available())
      return;
};


void formatEEPROM() {
   uint8_t b0 = EEPROM.read(0);
   uint8_t b1 = EEPROM.read(1);
   if ((b0 != EEP_FRM_1ST) && (b1 != EEP_FRM_2ND)) {
      Serial.println(F("EEPROM_NOT_FORMATED\nformatting..."));
      for (int i = 0 ; i < EEPROM.length(); i++)
         EEPROM.write(i, NULL);
      EEPROM.write(0, EEP_FRM_1ST);
      EEPROM.write(1, EEP_FRM_2ND);
      Serial.println(F("FORMATED"));
   } else {
     Serial.println(F("EEPROM_SEEMS_FORMATTED")); 
   }
};

/* #define EEP_SSX_LEN                       18
   #define EEP_SS1_ADR                       32
   #define EEP_SS2_ADR                       52
   #define EEP_SS3_ADR                       72
   #define EEP_SS4_ADR                       92
   #define EEP_SS5_ADR                       112
   #define EEP_SS6_ADR                       132
   #define EEP_SSX_END                       152 */
void displayConfigInfo() {
   __FlashStringHelper* hdr = F("-- [ ConfigInfo ] --"); 
   Serial.println(hdr);
   for (uint8_t i = 0; i < sizeof(SSX_LBL_TBL); i++) {
      memset(TMP_BUFF_32, NULL, sizeof(TMP_BUFF_32));
      uint8_t eepidx = SSX_LBL_TBL[i];
      for (uint8_t l = 0; l < 20; l++) {
         TMP_BUFF_32[l] = EEPROM.read((eepidx + l)); 
      }
      /* -- */
      uint8_t lcdln = 0;
      __LCD.clear();
      __LCD.setCursor(0, lcdln);
      __LCD.print(hdr);
      switch (eepidx) {
         case EEP_SS1_ADR:
            __LCD.setCursor(0, ++lcdln);
            __LCD.print(F("SS1: "));
            break;
         case EEP_SS2_ADR:
            __LCD.setCursor(0, ++lcdln);
            __LCD.print(F("SS2: "));
            break;
         case EEP_SS3_ADR:
            __LCD.setCursor(0, ++lcdln);
            __LCD.print(F("SS3: "));
            break;
         case EEP_SS4_ADR:
            __LCD.setCursor(0, ++lcdln);
            __LCD.print(F("SS4: "));
            break;
         case EEP_SS5_ADR:
            __LCD.setCursor(0, ++lcdln);
            __LCD.print(F("SS5: "));
            break;
         case EEP_SS6_ADR:
            __LCD.setCursor(0, ++lcdln);
            __LCD.print(F("SS6: "));
            break;
         default:
            Serial.print(F("BadSSX_ADR"));
            break;
      }
      /* -- */
      __LCD.setCursor(1, ++lcdln);
      __LCD.print(TMP_BUFF_32);
      delay(1000);
   }
};


#include <EEPROM.h>
#include "global.h"
#include "core.h"
#include "pezpxy.h"


PezProxy::PezProxy() { 
   this->errors = 0;
};


void PezProxy::Clear() {
   this->errors = 0;
   this->errcnt = 0;
   this->amps = 0.0;
   this->kwhrs = 0.0;
   this->freq = 0.0;
   this->volts = 0.0;
   this->watts = 0.0;
};


void PezProxy::InitLCD(uint8_t msgID = 0) {
   /* -- */
   auto h0 = [] () {
         __LCD.setCursor(0, 0);
         __LCD.print(model());
         __LCD.print(F(": v.:"));
         __LCD.print(ver());
      };
   /* -- */
   __LCD.clear();
   switch (msgID) {
      case 0:
         __LCD.setCursor(0, 0);
         __LCD.print(F("PZEM: "));
         __LCD.print(this->pins->name);
         __LCD.setCursor(1, 1);
         __LCD.print(F("MODE: "));
         __LCD.print(currentRunMode());
         break;
      case 1:
         __LCD.setCursor(3, 1);
         __LCD.print(F("-- welcome --"));
         break;
      case 2:
         h0();
         __LCD.setCursor(2, 1);
         __LCD.print(F("MODE: "));
         __LCD.print(currentRunMode());
         break;
      case 3:
         __LCD.setCursor(0, 0);
         __LCD.print(F("PZEM: "));
         __LCD.print(this->pins->name);
         __LCD.setCursor(1, 1);
         __LCD.print(F("MODE: "));
         __LCD.print(currentRunMode());
         break;
      case 4:
         h0();
         __LCD.setCursor(2, 1);
         __LCD.print(F("MODE: Wakeup"));
         break;
      case 5:
         h0();
         __LCD.setCursor(2, 1);
         __LCD.print(F("MODE: RedoDiag"));
         break;
      default:
         __LCD.setCursor(3, 0);
         __LCD.print(F("-- BadMsgID --"));
         break;
   }
};


void PezProxy::SetPins(SoftUartPins* pins) {
   this->pins = pins;
};


void PezProxy::SetPzem(PZEM004Tv30* pzem) {
   this->pzem_v3 = pzem;
};


void PezProxy::ReportHead() {
   __LCD.clear();
   __LCD.setCursor(0, 0);
   __LCD.print(F("Report: PZEM "));
   __LCD.print(this->pins->name);
};


void PezProxy::ErrorReport() {
   uint8_t lcdln = 2;
   uint8_t errCode = 0;
   /* line 0 */
   this->ReportHead();
   __LCD.setCursor(1, 1);
   __LCD.print(F("ReadErrCnt: "));
   __LCD.print(this->errcnt);
   __LCD.setCursor(1, lcdln);
   /* print errors */
   for (uint8_t i = 0; i < 5; i++) {
      errCode = (this->errors & READ_ERRS[i]);
      if (errCode == READ_ERRS[i]) {
         /* -- shift lcd line -- */
         if (i == 3)
            __LCD.setCursor(1, ++lcdln);
         /* -- */
         __LCD.print(ReadErrorMsg(errCode));
         __LCD.print(" ");
      }
   }
   /* - - */
};


void PezProxy::GoodReport() {
   /* line 0 */
   bool is_kW = false;
   this->ReportHead();
   /* == line 1 :: freq & volts */
   this->ClearTmps();
   dtostrf(this->freq, 0, 1, TMP_FLT_0);
   dtostrf(this->volts, 0, 1, TMP_FLT_1);
   sprintf(TMP_BUFF_32, "F: %s - V: %s", TMP_FLT_0, TMP_FLT_1);
   __LCD.setCursor(1, 1);
   __LCD.print(TMP_BUFF_32);
   /* == line 2 :: amps & watts */
   this->ClearTmps();
   dtostrf(this->amps, 0, 1, TMP_FLT_0);
   float kWDivVal = (this->watts > 1000) ? 1000 : 1;
   dtostrf((this->watts / kWDivVal) , 0, 1, TMP_FLT_1);
   if (kWDivVal == 1000) {
      sprintf(TMP_BUFF_32, "A: %s - kW: %s", TMP_FLT_0, TMP_FLT_1);
   } else {
      sprintf(TMP_BUFF_32, "A: %s - W: %s", TMP_FLT_0, TMP_FLT_1);
   }
   __LCD.setCursor(1, 2);
   __LCD.print(TMP_BUFF_32);
   /* == line 3 :: kwhrs */
   __LCD.setCursor(1, 3);
   __LCD.print(F("kWh: "));
   __LCD.print(this->kwhrs);
};


void PezProxy::PrintToLCD() {
   if (this->errcnt) { 
      this->ErrorReport();
   } else {
      this->GoodReport();
   }
};


void PezProxy::Run(bool withDataUpstreamPush) {
   this->errors = 0;
   /* freq */
   this->InitLCD();
   __LCD.setCursor(2, 2);
   __LCD.print(F("Read: Freq."));
   __LCD.setCursor(4, 3);
   if (this->read_Freq()) {
      __LCD.print(this->freq);
   } else {
      __LCD.print(F("ReadError"));
   }
   this->MonitorSystem(10, 100);
   /* volts */
   this->InitLCD();
   __LCD.setCursor(2, 2);
   __LCD.print("Read: Volts");
   __LCD.setCursor(4, 3);
   if (this->read_Volts()) {
      __LCD.print(this->volts);
   } else {
      __LCD.print(F("ReadError"));
   }
   this->MonitorSystem(10, 100);
   /* amps */
   this->InitLCD();
   __LCD.setCursor(2, 2);
   __LCD.print("Read: Amps");
   __LCD.setCursor(4, 3);
   if (this->read_Amps()) {
      __LCD.print(this->amps);
   } else {
      __LCD.print(F("ReadError"));
   }
   this->MonitorSystem(10, 100);
   /* watts */
   this->InitLCD();
   __LCD.setCursor(2, 2);
   __LCD.print("Read: Watts");
   __LCD.setCursor(4, 3);
   if (this->read_Watts()) {
      __LCD.print(this->watts);
   } else {
      __LCD.print(F("ReadError"));
   }
   this->MonitorSystem(10, 100);
   /* kWhrs */
   this->InitLCD();
   __LCD.setCursor(2, 2);
   __LCD.print("Read: kWh");
   __LCD.setCursor(4, 3);
   if (this->read_kWh()) {
      __LCD.print(this->kwhrs);
   } else {
      __LCD.print(F("ReadError"));
   }
   this->MonitorSystem(10, 100);
   /* print to LCD */
   this->PrintToLCD();
   /* hold report on lcd */
   this->MonitorSystem(20, 100);
   /* if true push up over usb/uart0 serial upstream for now */
   if ((CURRENT_ACTION == Diag) && (this->errors)) {
      this->InitLCD(3);
      __LCD.setCursor(1, 2);
      __LCD.print(F("UploadDiagErrors"));
      this->push_err_Upstream();
      __LCD.setCursor(4, 3);
      __LCD.print(F("Done!"));
   }
   /* -- */
   if (CURRENT_ACTION == ReadPush) {
      this->InitLCD(3);
      __LCD.setCursor(1, 2);
      __LCD.print(F("UploadReadResults"));
      if (withDataUpstreamPush && (this->errors == 0)) {
         this->push_ok_Upstream();
      } else if (withDataUpstreamPush && (this->errors)) {
         this->push_err_Upstream();
      }
      __LCD.setCursor(4, 3);
      __LCD.print(F("Done!"));
   }
   /* -- */
   this->MonitorSystem(10, 100);
};


bool PezProxy::push_ok_Upstream() {
   Serial.print(F("#RPT|PZEM:"));
   Serial.print(this->pins->name);
   /* freq */
   Serial.print(F("|F:"));
   Serial.print(this->freq);
   /* volts */
   Serial.print(F("|V:"));
   Serial.print(this->volts);
   /* amps */
   Serial.print(F("|A:"));
   Serial.print(this->amps);
   /* W */
   Serial.print(F("|W:"));
   Serial.print(this->watts);
   /* kWhrs */
   Serial.print(F("|kWh:"));
   Serial.print(this->kwhrs);
   /* send end char */
   Serial.println("!");
   return true;
};


bool PezProxy::push_err_Upstream() {   
   Serial.print(F("#ERR|PZEM:"));
   Serial.print(this->pins->name);
   /* run mode */
   Serial.print(F("|MODE:"));
   Serial.print(currentRunMode());
   /* error flags */
   Serial.print(F("|ERRFLGS:"));
   Serial.print(this->errors);
   Serial.println("!");
   return true;
};


void PezProxy::ClearTmps() {
   memset(TMP_FLT_0, 0, sizeof(TMP_FLT_0));
   memset(TMP_FLT_1, 0, sizeof(TMP_FLT_1));
   memset(TMP_BUFF_32, 0, sizeof(TMP_BUFF_32));
};


void PezProxy::CheckButtons() {
   /* btn 1 */
   if (__BTN_1.IsOn())
      __BTN_1.Pushed();
   /* btn 2 */
   if (__BTN_2.IsOn())
      __BTN_2.Pushed();
   /* - - */
   __BTN_1.Run();
   __BTN_2.Run();
};


void PezProxy::CheckComPort() {
   if (Serial.available() == 0)
      return;
   /* else read bytes; all incoming data has \n */
   String buff = Serial.readString();
   if (buff.startsWith("#") && (buff.c_str()[(buff.length() - 2)] == '!')) {
      Serial.print(F("OK_"));
      Serial.println(buff.length());
      this->ProcessGoodBuff(buff);
   } else {
      Serial.println(F("BAD_INPUT"));
   }
};


void PezProxy::MonitorSystem(uint8_t cnt, uint8_t cntms) {
   for (uint8_t i = 0; i < cnt; i++) {
      this->CheckButtons();
      this->CheckComPort();
      delay(cntms);
   }
};

/*
   #define EEP_SSX_LEN                       18
   #define EEP_SS1_ADR                       32
   #define EEP_SS2_ADR                       52
   #define EEP_SS3_ADR                       72
   #define EEP_SS4_ADR                       92
   #define EEP_SS5_ADR                       112
   #define EEP_SS6_ADR                       132
*/
void PezProxy::ProcessGoodBuff(String s) {
   /* -- */
   char c;
   uint8_t posS = 0;
   uint8_t posE = 0;
   uint16_t eeploc = 0;
   /* -- */
   if (s.startsWith(F("#SET|SS:"))) {
      posS = 8;
      posE = s.indexOf('|', posS);
      String t = s.substring(posS, posE);
      /* - - */
      if (t.equals(F("SS_1")))
         eeploc = EEP_SS1_ADR;
      else if (t.equals(F("SS_2")))
         eeploc = EEP_SS2_ADR;
      else if (t.equals(F("SS_3")))
         eeploc = EEP_SS3_ADR;
      else if (t.equals(F("SS_4")))
         eeploc = EEP_SS4_ADR;
      else if (t.equals(F("SS_5")))
         eeploc = EEP_SS5_ADR;
      else if (t.equals(F("SS_6")))
         eeploc = EEP_SS6_ADR;
      else
         eeploc = 8000;
      /* -- */
      if (eeploc != 8000) {
         posS = (s.indexOf(F("LBL:"), posE) + 4);
         s = s.substring(posS, s.indexOf('!'));
         memset(TMP_BUFF_32, NULL, sizeof(TMP_BUFF_32));
         memcpy(TMP_BUFF_32, s.c_str(), s.length());
         if (s.length() <= EEP_SSX_LEN) {
            for (uint8_t i = 0; i < 20; i++)
               EEPROM.write((eeploc + i), TMP_BUFF_32[i]);
         }
      } else {
         /* error */
      }
   } else if (s.startsWith(F("#GET|SS:"))) {
      Serial.println(F("#GET|SS:"));
      uint16_t eeploc = 0;
      uint8_t posS = (s.indexOf(':', posS) + 1);
      uint8_t posE = s.indexOf('!');
      String t = s.substring(posS, posE);
      Serial.println(t);
      /* - - */
      if (t.equals(F("SS_1")))
         eeploc = EEP_SS1_ADR;
      else if (t.equals(F("SS_2")))
         eeploc = EEP_SS2_ADR;
      else if (t.equals(F("SS_3")))
         eeploc = EEP_SS3_ADR;
      else if (t.equals(F("SS_4")))
         eeploc = EEP_SS4_ADR;
      else if (t.equals(F("SS_5")))
         eeploc = EEP_SS5_ADR;
      else if (t.equals(F("SS_6")))
         eeploc = EEP_SS6_ADR;
      else
         eeploc = 8000;
      /* -- */
      if (eeploc != 8000) {
         Serial.print(F("#RSP|"));
         Serial.print(t);
         Serial.print(F(":"));
         for (uint8_t i = 0; i < 20; i++) {
            c = EEPROM.read((eeploc + i));
            if (c == NULL)
               break;
            Serial.print(c);
         }
         Serial.println(F("!"));
      } else {
         /* error */
      }
      /* -- */
   } else if (s.indexOf(F("#GET|DEVINFO!")) == 0) {
      Serial.print(F("#DEVINFO|"));
      Serial.print(model());
      Serial.print(F("|"));
      Serial.print(ver());
      Serial.println(F("!"));
   } else if (s.indexOf(F("#GET|DEVSN!")) == 0) {
      Serial.print(F("#DEVINFO|DEVSN:"));
      for (uint16_t i = EEP_DEV_SN_START; i < EEP_DEV_SN_END; i++) {
         c = EEPROM.read(i);
         if (c == NULL)
            break;
         Serial.print(c);
      }
      Serial.println(F("!"));
   } else if (s.startsWith(F("#SET|DEVSN:"))) {
      posS = 11;
      s = s.substring(posS, s.indexOf('!'));
      memset(TMP_BUFF_32, NULL, sizeof(TMP_BUFF_32));
      memcpy(TMP_BUFF_32, s.c_str(), s.length());
      if (s.length() <= (EEP_DEV_SN_END - EEP_DEV_SN_START)) {
         for (uint8_t i = 0; i < s.length(); i++)
            EEPROM.write((EEP_DEV_SN_START + i), TMP_BUFF_32[i]);
      } else {
         Serial.println(F("SN_OVERFLOW"));         
      }
      /* -- */
      Serial.println(F("OK")); 
   }
};


bool PezProxy::read_Amps() {
   this->amps = this->pzem_v3->current();
   if (isnan(this->amps)) {
      this->errors |= READ_AMPS;
      this->errcnt++;
      return false;
   }
   /* -- */
   return true;
};

bool PezProxy::read_Volts() {
   this->volts = this->pzem_v3->voltage();
   if (isnan(this->volts)) {
      this->errors |= READ_VOLTS;
      this->errcnt++;
      return false;
   }
   /* -- */
   return true;
};

bool PezProxy::read_Watts() {
   this->watts = this->pzem_v3->power();
   if (isnan(this->watts)) {
      this->errors |= READ_WATTS;
      this->errcnt++;
      return false;
   } 
   /* -- */
   return true;
};

bool PezProxy::read_kWh() {
   this->kwhrs = this->pzem_v3->energy();
   if (isnan(this->kwhrs)) {
      this->errors |= READ_ENERGY;
      this->errcnt++;
      return false;
   }
   /* -- */
   return true;
};

bool PezProxy::read_Freq() {
   this->freq = this->pzem_v3->frequency();
   if (isnan(this->freq)) {
      this->errors |= READ_FREQ;
      this->errcnt++;
      return false;
   }
   /* -- */
   return true;
};

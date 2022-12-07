

#ifndef _CORE_
#define _CORE_

   #define COM_BAUDRATE                      19200
   #define SOFT_UART_CNT                     6
   #define LCD_ADDRESS                       0x27
   #define LCD_LINES                         4
   #define LCD_CHAR_PER_LINE                 20
   #define ERROR_OK                          0
   #define READ_DELAY                        1000
   #define BTN_1                             A2
   #define BTN_2                             A3
   #define MINUTES_6_INTV                    360000
   #define MINUTES_5_INTV                    300000
   #define MINUTES_2_INTV                    120000

   /* eeprom table; ss lbl is max 18 chars + 2 nulls */
   #define EEP_FRM_1ST                       0xAA
   #define EEP_FRM_2ND                       0x55
   #define EEP_SSX_LEN                       18
   #define EEP_SS1_ADR                       32
   #define EEP_SS2_ADR                       52
   #define EEP_SS3_ADR                       72
   #define EEP_SS4_ADR                       92
   #define EEP_SS5_ADR                       112
   #define EEP_SS6_ADR                       132
   #define EEP_SSX_END                       152
   uint8_t SSX_LBL_TBL[] = {EEP_SS1_ADR, EEP_SS2_ADR, EEP_SS3_ADR
      , EEP_SS4_ADR, EEP_SS5_ADR, EEP_SS6_ADR};
   #define EEP_DEV_SN_START                   154
   #define EEP_DEV_SN_END                     164


   enum LcdFrames {Sys, Pop};
   enum SysErrors {OK, ERR_0, ERR_1};
   enum ActionName {None, Init, Sleep, PzemRead, ReadPush
      , Diag, PushDataUp, PushErrorUp};
   enum ReadErrors {READ_WATTS = 2, READ_AMPS = 4, READ_VOLTS = 8
      , READ_FREQ = 16, READ_ENERGY = 32};

   //bool TIME_REPORT_UPSTREAM = false;
   char TMP_FLT_0[12];
   char TMP_FLT_1[12];
   char TMP_BUFF_32[32];
   char ERROR_MSG[21];
   uint8_t READ_ERRS[] = {READ_WATTS, READ_AMPS, READ_VOLTS
      , READ_FREQ, READ_ENERGY};
   bool BREAK_PINS_LOOP = false;

   /* D pins */
   uint8_t D02 = 0x02;    /*  rx  */
   uint8_t D03 = 0x03;    /*  tx  */
   uint8_t D04 = 0x04;
   uint8_t D05 = 0x05;
   uint8_t D06 = 0x06;
   uint8_t D07 = 0x07;
   uint8_t D08 = 0x08;
   uint8_t D09 = 0x09;
   uint8_t D10 = 0x0A;
   uint8_t D11 = 0x0B;
   /* A pins */
   uint8_t A00 = A0;
   uint8_t A01 = A1;
   uint8_t A02 = A2;
   uint8_t A03 = A3;
   uint8_t A04 = A4;
   uint8_t A05 = A5;


   struct SoftUartPins {
      uint8_t rxPin;
      uint8_t txPin;
      uint8_t id;
      char* name;
   };

   SoftUartPins _su1 = {D08, D09, 10, "SS_1"};     /* ok */
   SoftUartPins _su2 = {D06, D07, 12, "SS_2"};
   SoftUartPins _su3 = {D04, D05, 14, "SS_3"};     /* ok */
   SoftUartPins _su4 = {D02, D03, 16, "SS_4"};
   SoftUartPins _su5 = {D10, D11, 18, "SS_5"};
   SoftUartPins _su6 = {A00, A01, 20, "SS_6"};
   /* not used */
   // SoftUartPins _su7 = {A03, A02, "SU7"};
   // SoftUartPins _su8 = {A05, A04, "SU8"};

   struct SoftUartPins softUartsArr[] = {_su1, _su2, _su3, _su4, _su5, _su6};
   void InitSoftUarts();

   /* lcd stuff */
   char SYS_FRM[4][20];
   char POP_FRM[4][30];
   void InitLCD();
   void lcd_printPopFrm();
   void lcd_printSysFrm();
   LiquidCrystal_I2C __LCD(LCD_ADDRESS, LCD_CHAR_PER_LINE, LCD_LINES);  

   /* read error coes */
   __FlashStringHelper* ReadErrorMsg(ReadErrors errCode);
   
   void runOnAllPezmPorts();
   bool isTimeToRunUpstreamPush();
   bool isTimeToRunDiag();
   __FlashStringHelper* currentRunMode();
   __FlashStringHelper* ver();
   __FlashStringHelper* model();
   char* serialnumber();


   /* buttons */
   void btn1_OnPush();
   void btn2_OnPush();
   void btn1_OnRun(Button* pBtn);
   void btn2_OnRun(Button* pBtn);

   /* com */
   void checkComPort();

   /* sys */
   void formatEEPROM();
   void displayConfigInfo();
   void checkUnitAddress(SoftUartPins*);

#endif

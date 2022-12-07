

#ifndef __PEZPXY__
#define __PEZPXY__


   class PezProxy {

      private:
         
         SoftUartPins* pins;
         PZEM004Tv30* pzem_v3;
         float volts;
         float amps;
         float kwhrs;
         float freq;
         float watts;
         uint8_t errcnt;
         uint16_t errors;
         /* methods */
         bool read_Volts();
         bool read_Amps();
         bool read_Watts();
         bool read_kWh();
         bool read_Freq();
         bool push_ok_Upstream();
         bool push_err_Upstream();

      public:

         PezProxy();
         void SetPins(SoftUartPins* pins);
         void SetPzem(PZEM004Tv30* pzem);
         void Read(bool checkBtns = false);
         /* read from pzem and push upstream */
         void Run(bool withPush);
         void ReportHead();
         void GoodReport();
         void ErrorReport();
         void PrintToLCD();
         void ReadToLCDRx0Tx0();
         void ReportDataUpstream();
         void InitLCD(uint8_t msgID = 0);
         void Clear();
         void ClearTmps();
         void CheckButtons();
         void CheckComPort();
         void ProcessGoodBuff(String s);
         void MonitorSystem(uint8_t cnt, uint8_t cntms);
         void Btn1_On();
         void Btn2_On();

   };

#endif


#ifndef __button__
#define __button__


class Button {

   private:

      bool state = false;
      bool enabled = true;
      void (*_onPushRun)(void);
      void (*_onRunRun)(Button*);

   protected:

      uint8_t pin;
      char* name;
      unsigned long lastPushed = 0;

   public:

      Button(uint8_t pin, char* name);
      bool IsOn();
      void Pushed();
      void Run();
      void SetEnabled(bool);
      void onPushRun(void (*callback)(void));
      void onRunRun(void (*callback)(Button*));
      void PushPressedEvent();
      char* getName();
      uint32_t getLastPushed();

};

#endif

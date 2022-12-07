
#include "core.h"
#include "button.h"

/*
   use input_pullups input with buttons going to gnd
*/
Button::Button(uint8_t pin, char* name) {
   this->pin = pin;
   this->name = name;
   pinMode(this->pin, INPUT_PULLUP);
   this->lastPushed = 0;
};

bool Button::IsOn() {
   return (digitalRead(this->pin) == LOW);  
};

void Button::Pushed() {
   if (!this->enabled)
      return;
   /* debouce */
   if ((millis() - this->lastPushed) < 480)
      return;
   /* - - */
   this->PushPressedEvent();
   if (this->_onPushRun != NULL) {
      this->lastPushed = millis();
      this->_onPushRun();
   }
};

void Button::PushPressedEvent() {
   Serial.print("#EVT|MODE:");
   Serial.print(currentRunMode());
   Serial.print("|BTN:");
   Serial.print(this->name);
   Serial.print("|E:ON");
   Serial.println("!");
};

void Button::onPushRun(void (*callback)()) {
   this->_onPushRun = callback;
};


void Button::onRunRun(void (*callback)(Button*)) {
   this->_onRunRun = callback;
};

char* Button::getName() {
   return this->name;
};

uint32_t Button::getLastPushed() {
   return this->lastPushed;
};

void Button::Run() {
   /* - - */
   if ((!this->enabled) || (this->lastPushed == 0))
      return;
   /* - - */
   if (this->_onRunRun != NULL)
      this->_onRunRun(this);
   /* - - */
};

void Button::SetEnabled(bool v) {
   this->enabled = v;
};

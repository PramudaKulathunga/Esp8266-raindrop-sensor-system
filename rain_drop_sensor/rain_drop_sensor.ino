# define led 9
#include <Arduino.h>

#include "Talkie.h"
#include "Vocab_Special.h"
#include "Vocab_US_Acorn.h"
#include "Vocab_US_Large.h"
#include "Vocab_US_TI99.h"

Talkie voice;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)  || defined(ARDUINO_attiny3217)
  delay(4000);
#endif
#if defined(TEENSYDUINO)
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); //Enable Amplified PROP shield
#endif
}

void loop() {
  int sensor = analogRead(A0);
  int range = map(sensor, 0, 1023, 0, 3);
  Serial.println(range);

  if (range == 0) {

    for (int i = 0; i < 256; i += 3) {
      analogWrite(led, i);
      delay(5);
    }
    for (int i = 255; i > 0; i -= 3) {
      analogWrite(led, i);
      delay(5);
    }
    for (int i = 0; i < 256; i += 3) {
      analogWrite(led, i);}

      voice.say(spt_HURRY);
      voice.say(spt_UP);
      voice.say(spt_HURRY);
      voice.say(spt_UP);
      delay(100);
      voice.say(sp4_RAIN);
      voice.say(spa__ING);
      voice.say(sp4_START);
      voice.say(sp4_RAIN);
      voice.say(spa__ING);
      voice.say(sp4_START);
      delay(100);
      voice.say(sp4_ACTION);
      voice.say(spt_NEED);
      voice.say(sp4_IMMEDIATELY);
    
  }
  else {
    digitalWrite(led, LOW);
  }
}

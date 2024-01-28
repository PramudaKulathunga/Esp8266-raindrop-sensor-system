#include "CTBot.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"
#include "music.h"

CTBot myBot;
CTBotReplyKeyboard myKbd;
CTBotReplyKeyboard myKbd2;
CTBotInlineKeyboard myKbd3;
bool isKeyboardActive;

AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
AudioFileSourcePROGMEM *file2;
AudioOutputI2SNoDAC *out;

#define SHOW_KEYBOARD "Show keyboard"

String ssid = ""; //Enter your ssid
String pass = ""; //Enter your password
String token = ""; //Enter telegram channel token
uint8_t led1 = D1;
int rainSensor;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");

  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");

  pinMode(led1, OUTPUT);
  digitalWrite(led1, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);

  audioLogger = &Serial;
  file = new AudioFileSourcePROGMEM( music, sizeof(music) );
  file2 = new AudioFileSourcePROGMEM( music2, sizeof(music2) );
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();

  myKbd.addButton("✅ CHECK");
  myKbd.addButton("🚨 ALARM");
  myKbd.addButton("📄 INFO");
  myKbd.addRow();
  myKbd.addButton("❌ Hide Keyboard ❌");
  myKbd.enableResize();
  isKeyboardActive = false;

  myKbd2.addRow();
  myKbd2.addButton("🛑 STOP 🛑");
  myKbd2.enableResize();

  myKbd3.addRow();
  myKbd3.addButton("⚜ show keyboard ⚜", SHOW_KEYBOARD, CTBotKeyboardButtonQuery);
}

// for sensor reading
void moisture() {
  int rainSensorR = analogRead(A0);
  rainSensor = map(rainSensorR, 0, 1023, 0, 100);
  Serial.println(rainSensor);
}

//for led
void led() {
  int n = 0;
  while (n < 4) {
    for (int a = 0; a < 255; a += 3) {
      analogWrite(led1, a);
      delay(5);
    }
    for (int b = 255; b > 0; b -= 3) {
      analogWrite(led1, b);
      delay(5);
    }
    n++;
  }
  digitalWrite(led1, LOW);
}

void loop() {
  TBMessage msg;
  moisture();
  musicCheck();

  if (myBot.getNewMessage(msg)) {

    //for show keyboard
    if (msg.messageType == CTBotMessageText) {

      //for manual check whether
      if (msg.text.equalsIgnoreCase("✅ CHECK")) {
        if (rainSensor < 75) {
          myBot.sendMessage(msg.sender.id, "🌧 " + (String)(100 - rainSensor) + "% Rainning!! Need attention immediately!!");
          led();
          wav->begin(file, out);
        }
        else {
          digitalWrite(led1, LOW);
          myBot.sendMessage(msg.sender.id, "☀ There are Sunny whether!!");
          delay(100);
          wav->begin(file2, out);
        }
      }

      //for set alarm
      else if (msg.text.equalsIgnoreCase("🚨 ALARM")) {
        myBot.removeReplyKeyboard(msg.sender.id, "Alarm is running!!");
        isKeyboardActive = false;
        myBot.sendMessage(msg.sender.id, "If you want to stop alarm press STOP", myKbd2);

        while (rainSensor > 75) {
          moisture();
          if (myBot.getNewMessage(msg)) {
            if (msg.text.equalsIgnoreCase("🛑 STOP 🛑")) {
              rainSensor = 0;
            }
          }
        }
        moisture();
        myBot.removeReplyKeyboard(msg.sender.id, "Alarm stoped!!");
        if (rainSensor > 75) {
          myBot.sendMessage(msg.sender.id, "☀ There are Sunny whether!!", myKbd);
          wav->begin(file2, out);
        }
        else {
          myBot.sendMessage(msg.sender.id, "🌧 " + (String)(100 - rainSensor) + "% Rainning!! Need attention immediately", myKbd);
          led();
          wav->begin(file, out);
        }
        isKeyboardActive = true;
      }

      //for get info
      else if (msg.text.equalsIgnoreCase("📄 INFO")) {
        String reply;
        reply = (String)"Welcome " + msg.sender.username + (String)". \nSent like that.\n\n⚜ show keyboard ⚜ 👉🏻 (access vertual keyboard) \n✅ CHECK 👉🏻 (check whether condition)  \n🚨 ALARM 👉🏻 (turn on rainning alarm) \n📄 INFO 👉🏻 (get details about this bot)  \n❌ Hide Keyboard ❌ 👉🏻 (hide the keyboard) \n🛑 STOP 🛑 👉🏻 (turn off rainning alarm when you on it)";
        myBot.sendMessage(msg.sender.id, reply);
      }

      //for hide keyboord
      else if (isKeyboardActive) {
        if (msg.text.equalsIgnoreCase("❌ Hide Keyboard ❌")) {
          myBot.removeReplyKeyboard(msg.sender.id, "If you want start again sent any massege to proceed.");
          isKeyboardActive = false;
        } else {
          myBot.sendMessage(msg.sender.id, "Wrong command!!");
        }
      }

      else {
        myBot.sendMessage(msg.sender.id, "Press 'show keyboard' to proceed", myKbd3);
      }
    }

    else if (msg.messageType == CTBotMessageQuery) {
      if (msg.callbackQueryData.equals(SHOW_KEYBOARD)) {
        myBot.endQuery(msg.callbackQueryID, "Please wait...");
        myBot.sendMessage(msg.sender.id, "choose your action!!", myKbd);
        isKeyboardActive = true;
      }
    }
    delay(500);
  }
}

void musicCheck() {
  if (wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
      file = new AudioFileSourcePROGMEM( music, sizeof(music) );
      file2 = new AudioFileSourcePROGMEM( music2, sizeof(music2) );
      out = new AudioOutputI2SNoDAC();
      wav = new AudioGeneratorWAV();
    }
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }
}

#include "Adafruit_MCP23X17.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

// for wifi communcation: to handle time.
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// TODO: replace these with actual stuff
const char* ssid     = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

Preferences preferences;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

TwoWire wire0 = TwoWire(0);
TwoWire wire1 = TwoWire(1);

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &wire0, OLED_RESET); // display 1
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &wire1, OLED_RESET); // display 2

// PORTS ON THE MCP23017
// buttons
#define BTN_1 0
#define BTN_2 1
#define BTN_3 2
#define BTN_4 3

// PORTS ON THE ESP

// the GPIO extender
#define MCP23017SCK 19
#define MCP23017SDA 20

// buzzer
#define BZR 17

// leds
#define LED1 22
#define LED2 23
#define LED3 16

// screen 1
#define SDA1_PIN 0
#define SCL1_PIN 1

// screen 2
#define SDA2_PIN 2
#define SCL2_PIN 21

Adafruit_MCP23X17 mcp;

enum class STATES {
  STUDYING,
  CODING,
  SCROLLING,
  GAMING,
  OPTIONS
};

STATES cur_state = STATES::OPTIONS;

int timer = 0;

int scroll_count = 0;
int gaming_count = 0;
int score = 0;
// int time_elapsed = 0;
bool started = false;
int last_timer_update = 0;

// 100; 250; 500; 1,000; 5,000; 10,000, 100,000, 1,000,000
bool milestones[8] = {0, 0, 0, 0, 0, 0, 0, 0};

const unsigned char PROGMEM spriteOPTION[] = {
  0b00000000, 0b00000000,
  0b00001111, 0b11000000,
  0b00011000, 0b01100000,
  0b00010000, 0b00100000,
  0b00000000, 0b00100000,
  0b00000000, 0b00100000,
  0b00000000, 0b01100000,
  0b00000001, 0b11000000,
  0b00000001, 0b00000000,
  0b00000001, 0b00000000,
  0b00000001, 0b00000000,
  0b00000000, 0b00000000,
  0b00000001, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
};

const unsigned char PROGMEM spriteCODING[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000011, 0b00000000,
  0b00000110, 0b00000010,
  0b00001100, 0b00000110,
  0b00001000, 0b10001100,
  0b00001000, 0b00001000,
  0b00010000, 0b00001000,
  0b00010000, 0b10001000,
  0b00011000, 0b00011000,
  0b00001100, 0b00011000,
  0b00000111, 0b10011000,
  0b00000000, 0b11010000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
};

const unsigned char PROGMEM spriteSTUDYING[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b11100000,
  0b00000000, 0b10011000,
  0b00000001, 0b00001100,
  0b00000010, 0b00000110,
  0b00000110, 0b00001110,
  0b00000100, 0b00001110,
  0b00001000, 0b00001110,
  0b00001110, 0b00011110,
  0b00001111, 0b10111110,
  0b00001111, 0b11111110,
  0b00001111, 0b11111100,
  0b00000111, 0b11111000
};

const unsigned char PROGMEM spriteSCROLLING[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b00010000,
  0b00001111, 0b00111000,
  0b00011111, 0b01111000,
  0b00001111, 0b11111000,
  0b00000111, 0b11110000,
  0b00000011, 0b11100000,
  0b00000011, 0b11111000,
  0b00000111, 0b11111100,
  0b00001111, 0b11111110,
  0b00001111, 0b01111110,
  0b00001110, 0b00111100,
  0b00001100, 0b00001100,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
};

const unsigned char PROGMEM spriteGAMING[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00011111, 0b11111000,
  0b00010000, 0b00001100,
  0b00110110, 0b01100100,
  0b00100110, 0b01100010,
  0b00100000, 0b00000110,
  0b00111111, 0b11111100,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
};

// Usage:
// display.drawBitmap(x, y, sprite, 16, 16, SSD1306_WHITE);

// the tunes for the different milstones, with a starting note of varying durations.
void play100() {
  tone(BZR, 1500, 100);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}
void play250() {
  tone(BZR, 1500, 150);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}
void play500() {
  tone(BZR, 1500, 200);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}
void play1000() {
  tone(BZR, 1500, 250);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}
void play5000() {
  tone(BZR, 1500, 300);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}
void play10000() {
  tone(BZR, 1500, 400);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}
void play100000() {
  tone(BZR, 1500, 500);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}
void play1000000() {
  tone(BZR, 1500, 1000);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
  delay(300);
  tone(BZR, 1000, 50);
}

// getting all the different components ready.
void setup() {
  // get the values for amount of time
  preferences.begin("my-app", false);
  scroll_count = preferences.getInt("scrolls", 0);
  gaming_count = preferences.getInt("games", 0);
  preferences.end();

  // connect to the wifi for date and time
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  timeClient.begin();
  timeClient.setTimeOffset(0);

  // the gpio expander
  if (!mcp.begin_I2C()) {
    Serial.println("Error.");
    while (1);
  }

  mcp.pinMode(BTN_1, INPUT_PULLUP);
  mcp.pinMode(BTN_2, INPUT_PULLUP);
  mcp.pinMode(BTN_2, INPUT_PULLUP);
  mcp.pinMode(BTN_3, INPUT_PULLUP);

  // the screen.
  wire0.begin(SDA1_PIN, SCL1_PIN);
  wire1.begin(SDA2_PIN, SCL2_PIN);

  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display2.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // buzzer.
  pinMode(BZR, OUTPUT);

  // the LED
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

}

auto checkButtons() {
  if (!mcp.digitalRead(BTN_1)) {
    if(cur_state != STATES::STUDYING && timer > 0) {
      // cant change unless have completed the time. COULD CHANGE THIS LATER SO THAT IF WAS SCROLLING/GAMING CAN SWITCH.
      Serial.print("got to finish already allocated time first!");
    } else {
      cur_state = STATES::STUDYING;
      timer += 10 * 60; // add 10 minutes to the timer
    }
  } else if(!mcp.digitalRead(BTN_2)) {
    if(cur_state != STATES::CODING && timer > 0) {
      // cant change unless have completed the time. COULD CHANGE THIS LATER SO THAT IF WAS SCROLLING/GAMING CAN SWITCH.
      Serial.print("got to finish already allocated time first!");
    } else {
      cur_state = STATES::CODING;
      timer += 10 * 60; // add 10 minutes to the timer
    }
  } else if(!mcp.digitalRead(BTN_3)) {
    if(cur_state != STATES::SCROLLING && timer > 0) {
      // cant change unless have completed the time. COULD CHANGE THIS LATER SO THAT IF WAS SCROLLING/GAMING CAN SWITCH.
      Serial.print("got to finish already allocated time first!");
    } else if(cur_state == STATES::SCROLLING || scroll_count > 0) {
      // cant do more reels after 10 minutes!
      Serial.print("cant scroll for more than 10 minutes!");
    } else {
      cur_state = STATES::SCROLLING;
      timer += 10 * 60; // add 10 minutes to the timer
      scroll_count += 1;
      preferences.begin("my-app", false);
      preferences.putInt("scrolls", scroll_count);
      preferences.putString("lastScroll", dayStamp);
      preferences.end();
    }
  } else if(!mcp.digitalRead(BTN_4)) {
    if(cur_state != STATES::GAMING && timer > 0) {
      // cant change unless have completed the time. COULD CHANGE THIS LATER SO THAT IF WAS SCROLLING/GAMING CAN SWITCH.
      Serial.print("got to finish already allocated time first!");
    } else if(cur_state == STATES::GAMING || gaming_count > 2) {
      // cant do more reels after 10 minutes!
      Serial.print("cant scroll for more than 10 minutes!");
    } else {
      cur_state = STATES::GAMING;
      timer += 10 * 60; // add 10 minutes to the timer
      gaming_count += 1;
      preferences.begin("my-app", false);
      preferences.putInt("games", gaming_count);
      preferences.putString("lastGame", dayStamp);
      preferences.end();
    }
  }
}

auto updateAndLogic() {
  // time synced with the wifi...
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // string manip :()
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  delay(1000);
  auto idx_1 = formattedDate.indexOf(":");
  String hours_ = formattedDate.substring(0, idx_1);
  String rest = formattedDate.substring(idx_1+1, formattedDate.length()-1);
  auto idx_2 = rest.indexOf(":");
  String minutes_ = rest.substring(0, idx_2);
  String seconds_ = rest.substring(idx_2+1, rest.length()-1);
  int hours = hours_.toInt();
  int minutes = minutes_.toInt();
  int seconds = seconds_.toInt();

  int seconds_elapsed_today = hours * 60 * 60 + minutes * 60 + seconds;
  if(!started) {
    int seconds_started = seconds_elapsed_today;
    started = true;
    last_timer_update = seconds_elapsed_today;
  }

  // if the day stamp has changed, then we reset scroll_count and gaming_count to 0
  preferences.begin("my-app", false);
  String scroll = preferences.getString("lastScroll", dayStamp);
  String game = preferences.getString("lastGame", dayStamp);

  preferences.end();

  // this ensures the games/scrolls gets reset after each day.
  if(scroll != dayStamp) {
    scroll_count = 0;
  }
  if (game != dayStamp) {
    gaming_count = 0;
  }


  // for now im just going to use delay, there is going to be a better way..
  if(timer > 0) {
    //delay(1000);
    //time_elapsed += 1;
    // we reset limits after one day, however this requires it being plugged in, storing is too complicated as have to know the date and everything...
    // if(time_elapsed >= 24 * 60 * 60) {
    // }
    if(timer == 1) {
      // the we have finished the 10 minutes, do a buzzer sound!
      tone(BZR, 1000, 100);
      cur_state = STATES::OPTIONS;
    }
    if(timer % (10*60) == 1) {
    // depending on the acitvity we will add/deduct points, as will have finished another 10 minutes
      switch(cur_state) {
        case STATES::STUDYING:
          score += 10;
          break;
        case STATES::CODING:
          score += 15;
          break;
        case STATES::SCROLLING:
          score -= 20;
          break;
        case STATES::GAMING:
          score -= 5;
          break;
      }
    }
    timer -= seconds_elapsed_today - last_timer_update; // we calculate the difference, we dont just do delay(1000); more accurate here, but may
                                                        // not do even count down with the timer display
    last_timer_update = seconds_elapsed_today;
  }
  // now check for highscores
  if(score >= 100 && milestones[0] == 0) {
    play100();
    milestones[0] = 1;
  }
  if(score >= 250 && milestones[1] == 0) {
    play250();
    milestones[1] = 1;
  }
  if(score >= 500 && milestones[2] == 0) {
    play500();
    milestones[2] = 1;
  }
  if(score >= 1000 && milestones[3] == 0) {
    play1000();
    milestones[3] = 1;
  }
  if(score >= 5000 && milestones[4] == 0) {
    play5000();
    milestones[4] = 1;
  }
  if(score >= 10000 && milestones[5] == 0) {
    play10000();
    milestones[5] = 1;
  }
  if(score >= 100000 && milestones[6] == 0) {
    play100000();
    milestones[6] = 1;
  }
  if(score >= 100000 && milestones[7] == 0) {
    play100000();
    milestones[7] = 1;
  }

  // update the LED.
  switch(cur_state) {
    case STATES::STUDYING:
      digitalWrite(LED1, HIGH);
      break;
    case STATES::CODING:
      digitalWrite(LED2, HIGH);
      break;
    case STATES::SCROLLING:
      digitalWrite(LED3, HIGH);
      break;
    case STATES::GAMING:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      //digitalWrite(LED3, HIGH);
      break;
    case STATES::OPTIONS:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);

  }
}


// TODO: MAKE IT MORE AESTHETICALLY PLEASING.
auto render() {
  // we have to render things on both screens.
  // first deal with the first screen. (the options) and the stats (the stats will show no matter the screen state.)
  display1.clearDisplay();
  const unsigned char* bitmap;
  switch(cur_state) {
    case STATES::OPTIONS:
      display1.setCursor(0, 56);
      display1.setTextSize(1);
      display1.println("[STUDY] [CODE] [SCRL] [GME]");
      bitmap = spriteOPTION;
      break;
    case STATES::STUDYING:
      display1.setCursor(0, 56);
      display1.setTextSize(2);
      display1.println("STUDYING");
      bitmap = spriteSTUDYING;
      break;
    case STATES::CODING:
      display1.setCursor(0, 56);
      display1.setTextSize(2);
      display1.println("CODING");
      bitmap = spriteCODING;
      break;
    case STATES::SCROLLING:
      display1.setCursor(0, 56);
      display1.setTextSize(2);
      display1.println("SCROLLING");
      bitmap = spriteSCROLLING;
      break;
    case STATES::GAMING:
      display1.setCursor(0, 56);
      display1.setTextSize(2);
      display1.println("GAMING");
      bitmap = spriteGAMING;
      break;
  }

  // draw the bitmap
  display1.drawBitmap(56, 2, bitmap, 16, 16, SSD1306_WHITE);

  // then all will display that stats
  display1.setCursor(0, 24);
  display1.setTextSize(1);
  display1.println("CURRENT POINTS: ");
  display1.setCursor(50, 24);
  display1.println(score);

  display1.display();

  // now the second screen, the timer.
  display2.clearDisplay();

  display2.setCursor(0, 30);
  display2.setTextSize(2);
  display2.println("TIME: ");
  display2.setCursor(50, 30);
  int minutes = (timer - (timer % 60)) / 60;
  int seconds = timer - (minutes * 60);
  display2.println(minutes);
  display2.setCursor(60, 30);
  display2.println(":");
  display2.setCursor(65, 30);
  display2.println(seconds);

  display2.display();
}

// auto handleScreenLogic() {
//   // here we need to deal with 2 screens!
// }

void loop() {
  checkButtons(); // check inputs
  updateAndLogic(); // update all stuff, check for new milstones and change time etc.
  // handleScreenLogic();  // 3. Process actions
  render(); // draw on screen
  // dont think we need it? however could reduce our version of counting down to 900ms from 1000ms.
  //delay(100);           // 5. Short pause
}

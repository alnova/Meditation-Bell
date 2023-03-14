#include <Encoder.h>
#include <LiquidCrystal.h>

const int
d7_pin = 2,
d6_pin = 3,
d5_pin = 4,
d4_pin = 5,
black_button_pin = 6,
charge_pin = 7,
fire_pin = 8,
enc_0 = 9,
enc_1 = 10,
en_pin = 11,
rs_pin = 12,
red_button_pin = 53;

enum {
  BUTTON_PRESSED = 0,
  BUTTON_NOT_PRESSED = 1,
};

#define NUMBER_OF_BEGINNING_GONGS	1
#define BEGINNING_GONG_SPACING		2
#define MAIN_TIMER			3
#define NUMBER_OF_ENDGONGS		4
#define END_GONG_SPACING		5
#define LAST_KNOB			5 // same number as last knob

int which_number_is_being_knobbed = 1;
int mode = 0;
unsigned long meditationStartTime = 0;

int Number_of_Beginning_Gongs = 1;
int Beginning_Gong_Spacing = 1;
int main_Timer = 1;
int Number_of_EndGongs = 1;
int End_Gong_Spacing = 1;

int blackButtonValue = 0;
int redButtonValue = 0;
int dialValue = 0;

int count = 0;
char line2[32];

LiquidCrystal lcd(rs_pin, en_pin, d4_pin, d5_pin, d6_pin, d7_pin);
Encoder myEnc(enc_0, enc_1);

void setup() {

  lcd.begin(16, 2);
  lcd.print("BG GS MT EG ES  ");
  
  Serial.begin(9600);
  Serial.println("Set Dial:");
  pinMode(black_button_pin, INPUT_PULLUP);
  pinMode(red_button_pin, INPUT_PULLUP);

  pinMode(charge_pin, OUTPUT);
  pinMode(fire_pin, OUTPUT);

  digitalWrite(charge_pin, LOW);
  digitalWrite(fire_pin, LOW);
}

void loop() {
  int newDialValue = myEnc.read()/4;
  if (newDialValue < 0) {
    newDialValue = 99;
    myEnc.write(99*4);
  }

  if (newDialValue > 99) {
    newDialValue = 0;
    myEnc.write(0);
  }

  bool dialChanged = false;

  char lcdtext[40];
  if (dialValue != newDialValue)
  {
    dialChanged = true;
    dialValue = newDialValue;
    Serial.println(dialValue);

    if (which_number_is_being_knobbed == NUMBER_OF_BEGINNING_GONGS)  lcd.setCursor(0, 1);
    if (which_number_is_being_knobbed == BEGINNING_GONG_SPACING)    lcd.setCursor(3, 1);
    if (which_number_is_being_knobbed == MAIN_TIMER)            lcd.setCursor(6, 1);
    if (which_number_is_being_knobbed == NUMBER_OF_ENDGONGS)    lcd.setCursor(9, 1);
    if (which_number_is_being_knobbed == END_GONG_SPACING)    lcd.setCursor(12, 1);
    sprintf(lcdtext, "%02d",dialValue);
    lcd.blink();
    lcd.print(lcdtext);
  }

  if (which_number_is_being_knobbed == NUMBER_OF_BEGINNING_GONGS)	Number_of_Beginning_Gongs = dialValue;
  if (which_number_is_being_knobbed == BEGINNING_GONG_SPACING)		Beginning_Gong_Spacing = dialValue;
  if (which_number_is_being_knobbed == MAIN_TIMER)			      main_Timer = dialValue;
  if (which_number_is_being_knobbed == NUMBER_OF_ENDGONGS)		Number_of_EndGongs = dialValue;
  if (which_number_is_being_knobbed == END_GONG_SPACING)		End_Gong_Spacing = dialValue;

  char str[256];

  int newBlackButtonValue = digitalRead(black_button_pin);
  bool blackButtonChanged = (blackButtonValue != newBlackButtonValue);
  blackButtonValue = newBlackButtonValue;
  if ((blackButtonChanged == true) && blackButtonValue == BUTTON_PRESSED)
  {
    sprintf(lcdtext, "%02d %02d %02d %02d %02d  ",Number_of_Beginning_Gongs,Beginning_Gong_Spacing,main_Timer,Number_of_EndGongs,End_Gong_Spacing);
    Serial.print(lcdtext);

    lcd.setCursor(0, 1);
    lcd.print(lcdtext);

    sprintf(str, "dialvalue: %d, "
            "buttvalue: %d, "
            "// knob: %d "
            "// gongs1: %d, "
            "spacing1: %d, "
            "mainTimer: %d, "
            "gongs2: %d, "
            "spacing2: %d\n", dialValue, blackButtonValue, which_number_is_being_knobbed, Number_of_Beginning_Gongs, Beginning_Gong_Spacing, main_Timer, Number_of_EndGongs, End_Gong_Spacing);
    Serial.print(str);

    which_number_is_being_knobbed += 1;
    if (which_number_is_being_knobbed > LAST_KNOB) {
      which_number_is_being_knobbed = 1;
    }
    if (which_number_is_being_knobbed == NUMBER_OF_BEGINNING_GONGS)  myEnc.write(4*Number_of_Beginning_Gongs);
    if (which_number_is_being_knobbed == BEGINNING_GONG_SPACING)    myEnc.write(4*Beginning_Gong_Spacing);
    if (which_number_is_being_knobbed == MAIN_TIMER)            myEnc.write(4*main_Timer);
    if (which_number_is_being_knobbed == NUMBER_OF_ENDGONGS)    myEnc.write(4*Number_of_EndGongs);
    if (which_number_is_being_knobbed == END_GONG_SPACING)    myEnc.write(4*End_Gong_Spacing);
    delay(250);
  }

  int newRedButtonValue = digitalRead(red_button_pin);
  bool redButtonChanged = (redButtonValue != newRedButtonValue);
  redButtonValue = newRedButtonValue;
  if ((redButtonChanged == true) && redButtonValue == BUTTON_PRESSED) {
    if (mode == 0) {
      meditationStartTime = millis();
      mode = 1;
    }
    else {
      mode=0;
    }
    Serial.print(mode);
    Serial.println(" you pressed the red button");
    delay(250);
  }
  if (mode != 0) meditate();//this is the statemachine
}

void meditate() {
  if (mode == 1) { // if mode is 1, then based on millis minus starttime, and BG and GS, do the beginning gongs, and when finished, change mode to 2
    for (int gongNum = 0; gongNum < Number_of_Beginning_Gongs; gongNum++) {
      unsigned long thisGongTime = gongNum * (Beginning_Gong_Spacing * 1000); // how long after start time this gong should happen
      while (mode == 1 && (millis() - meditationStartTime < thisGongTime)) { //wait until it's time to do this gong
        if (digitalRead(red_button_pin) == BUTTON_PRESSED) {
          mode = 0; // if button gets pressed, abort
          gongNum = Number_of_Beginning_Gongs; //end the for loop if red button is pressed
        }
      }
      Serial.println("firing gong number "+String(gongNum));
      fireGong(1000);
    }
    if (mode == 1) mode = 2;
  }

  //if mode is 2 compare millis minus start time with the mainTimer plus amount of time that BG took, and if it's past that time, change mode to 3
  unsigned long totalBGTime = Number_of_Beginning_Gongs * (Beginning_Gong_Spacing * 1000);
  unsigned long meditationTime = totalBGTime + (main_Timer * 60000);

  if (mode == 2) {
    while (mode == 2 && (millis() - meditationStartTime < meditationTime )) { //wait until end of mainTimer time
      if (digitalRead(red_button_pin) == BUTTON_PRESSED) mode = 0; // if button gets pressed, abort
    }
    if (mode == 2) mode = 3;
  }
  if (mode == 3) { // if mode is 3, then based on millis minus starttime, and BG and endTime, do the end gongs, and when finished, change mode to 4
    for (int gongNum = 0; gongNum < Number_of_EndGongs; gongNum++) {
      unsigned long thisGongTime = gongNum * (End_Gong_Spacing * 1000); // how long after start time this gong should happen
      while (mode == 3 && (millis() - meditationStartTime < (meditationTime + thisGongTime))) { //wait until it's time to do this gong
        if (digitalRead(red_button_pin) == BUTTON_PRESSED)  {
          mode = 0; // if button gets pressed, abort
          gongNum = Number_of_EndGongs; //end the for loop if red button is pressed
        }
      }
      Serial.println("firing gong number "+String(gongNum));
      fireGong(1000);
    }
    if (mode == 3) mode = 4;
  }
}

void fireGong(int intensity) {

  if (intensity > 2000 || intensity < 1) intensity = 2000;

  digitalWrite(charge_pin, HIGH);   // turn the pin on (HIGH is the voltage level)
  delay(intensity);
  digitalWrite(fire_pin, HIGH);
  delay(100);
  digitalWrite(charge_pin, LOW);
  delay(100);
  digitalWrite(fire_pin, LOW);
  delay(100);
}

void checkLCD() {
    // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  memset(line2, ' ', 32);
  sprintf(line2, "%d", count++);
  lcd.print(line2);
}

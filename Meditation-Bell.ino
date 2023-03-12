/* Encoder Library - Basic Example
   http://www.pjrc.com/teensy/td_libs_Encoder.html

   This example code is in the public domain.
*/

#include <Encoder.h>
#include <LiquidCrystal.h>

const int
d7_pin = 2,
d6_pin = 3,
d5_pin = 4,
d4_pin = 5,
button_pin = 6,
charge_pin = 7,
fire_pin = 8,
enc_0 = 9,
enc_1 = 10,
en_pin = 11,
rs_pin = 12;

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

int Number_of_Beginning_Gongs = 1;
int Beginning_Gong_Spacing = 1;
int main_Timer = 1;
int Number_of_EndGongs = 1;
int End_Gong_Spacing = 1;

int buttonValue = 0;
int dialValue = 0;

int count = 0;
char line2[32];

LiquidCrystal lcd(rs_pin, en_pin, d4_pin, d5_pin, d6_pin, d7_pin);
Encoder myEnc(enc_0, enc_1);

void setup() {

  lcd.begin(16, 2);
  lcd.print("alessandra!");
  
  Serial.begin(9600);
  Serial.println("Encoder Readout:");
  pinMode(button_pin, INPUT_PULLUP);

  pinMode(charge_pin, OUTPUT);
  pinMode(fire_pin, OUTPUT);

  digitalWrite(charge_pin, LOW);
  digitalWrite(fire_pin, LOW);
}

void loop() {
  int newDialValue = myEnc.read();
  if (newDialValue < 0) {
    newDialValue = 99;
    myEnc.write(99);
  }

  if (newDialValue > 99) {
    newDialValue = 0;
    myEnc.write(0);
  }

  bool dialChanged = false;

  int newButtonValue = digitalRead(button_pin);
  bool buttonChanged = false;

  if (buttonValue != newButtonValue)
  {
    buttonChanged = true;
  }

  buttonValue = newButtonValue;

  if (dialValue != newDialValue)
  {
    dialChanged = true;
    dialValue = newDialValue;
    Serial.println(dialValue);
  }

  if (which_number_is_being_knobbed == NUMBER_OF_BEGINNING_GONGS)	Number_of_Beginning_Gongs = dialValue;
  if (which_number_is_being_knobbed == BEGINNING_GONG_SPACING)		Beginning_Gong_Spacing = dialValue;
  if (which_number_is_being_knobbed == MAIN_TIMER)			      main_Timer = dialValue;
  if (which_number_is_being_knobbed == NUMBER_OF_ENDGONGS)		Number_of_EndGongs = dialValue;
  if (which_number_is_being_knobbed == END_GONG_SPACING)		End_Gong_Spacing = dialValue;

  char str[256];

  if ((buttonChanged == true) && buttonValue == BUTTON_PRESSED)
  {
    sprintf(str, "dialvalue: %d, "
            "buttvalue: %d, "
            "// knob: %d "
            "// gongs1: %d, "
            "spacing1: %d, "
            "mainTimer: %d, "
            "gongs2: %d, "
            "spacing2: %d\n", dialValue, buttonValue, which_number_is_being_knobbed, Number_of_Beginning_Gongs, Beginning_Gong_Spacing, main_Timer, Number_of_EndGongs, End_Gong_Spacing);
    Serial.print(str);

    which_number_is_being_knobbed += 1;
    if (which_number_is_being_knobbed > LAST_KNOB) {
      which_number_is_being_knobbed = 1;
    }
    delay(250);
  }

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

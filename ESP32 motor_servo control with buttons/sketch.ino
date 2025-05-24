#include <ESP32Servo.h>
#include <AccelStepper.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Bounce2.h>

byte button_pins[] = {27, 26}; // button pins, 4,5 = up/down, 6 = select
#define NUMBUTTONS sizeof(button_pins)
Bounce * buttons = new Bounce[NUMBUTTONS];

U8X8_SSD1306_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE);

#define MENU_SIZE 2
char *menu[MENU_SIZE] = { "Display Servo", "Display Motor" };

int cursor=0;

#define motorInterfaceType 1

const int DIR = 12;
const int STEP = 14;
const int steps_per_rev = 200;

Servo myservo;  // create servo object to control a servo

int angle =90;    // initial angle  for servo
int angleStep =5;

AccelStepper motor(motorInterfaceType, STEP, DIR);
int stepperSpeed = 1000;

#define LEFT 16   // pin 12 is connected to left button
#define RIGHT  17  // pin 2 is connected to right button

void setup() {
  Serial.begin(9600);          //  setup serial
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  motor.setMaxSpeed(2000);
  motor.setSpeed(stepperSpeed);
  motor.moveTo(10);

   for (int i=0; i<NUMBUTTONS; i++) {
    buttons[i].attach( button_pins[i], INPUT_PULLUP); // setup the bounce instance for the current button
    buttons[i].interval(25); // interval in ms
  }
  
  display.begin();
  display.setPowerSave(0);
  display.setFont(u8x8_font_pxplusibmcgathin_f);

  showMenu();

  myservo.attach(19);  // attaches the servo on pin 9 to the servo object
  pinMode(LEFT,INPUT_PULLUP); // assign pin 12 ass input for Left button
  pinMode(RIGHT,INPUT_PULLUP);// assing pin 2 as input for right button
  myservo.write(angle);// send servo to the middle at 90 degrees
}

void moveStepperClockwise() {
  if (motor.distanceToGo() == 0) {
    motor.moveTo(-motor.currentPosition());
    Serial.println("Rotating Motor in opposite direction...");
  }
  motor.run();
  stepperSpeed += 100;
  if (stepperSpeed > 2000) {
    stepperSpeed = 2000;
  } else {
    motor.setSpeed(stepperSpeed);
    Serial.print("Stepper Speed: ");
    Serial.println(stepperSpeed);
  }
}

void moveStepperCounterClockwise() {
  if (motor.distanceToGo() == 0) {
    motor.moveTo(-motor.currentPosition());
    Serial.println("Rotating Motor in opposite direction...");
  }
  motor.run();
  stepperSpeed -= 100;
  if (stepperSpeed <= 0) {
    stepperSpeed = 100;
  } else {
    motor.setSpeed(stepperSpeed);
    Serial.print("Stepper Speed: ");
    Serial.println(stepperSpeed);
  }
}

void updateServoAngleClockwise() {
  if (angle > 0 && angle <= 180) {
    angle = angle - angleStep;
    if (angle < 0) {
      angle = 0;
    } else {
      myservo.write(angle);
      Serial.print("Moved to: ");
      Serial.print(angle);
      Serial.println(" degree");
    }
  }
}

void updateServoAngleCounterClockwise() {
  if (angle >= 0 && angle <= 180) {
    angle = angle + angleStep;
    if (angle > 180) {
      angle = 180;
    } else {
      myservo.write(angle);
      Serial.print("Moved to: ");
      Serial.print(angle);
      Serial.println(" degree");
    }
  }
}

void loop() {
  motor.run();
  while (digitalRead(RIGHT) == LOW) {
    moveStepperClockwise();
    updateServoAngleClockwise();
    delay(100);
  }

  while (digitalRead(LEFT) == LOW) {
    moveStepperCounterClockwise();
    updateServoAngleCounterClockwise();
    delay(100);
  }

  for (int i = 0; i < NUMBUTTONS; i++) {
    buttons[i].update(); // Update the Bounce instance
    if (buttons[i].fell()) { // If it fell
      if (i == 1) { // select (assuming 26 is used for select)
        display.clearLine(5);
        display.clearLine(6);
        display.setCursor(0, 5);
        executeChoice(cursor);
      } else {
        // erase previous cursor:
        display.setCursor(0, cursor);
        display.print(' ');
        if (i == 0) { // down (assuming 27 is used for down)
          cursor++;
          if (cursor > (MENU_SIZE - 1))
            cursor = 0;
        } 
        // show cursor at the new line:
        display.setCursor(0, cursor);
        display.print('>');
      }
    } // 
  }
}

void showMenu() {
  cursor=0;
  display.clearDisplay();
  // show menu items:
  for (int i = 0; i<MENU_SIZE; i++) {
    display.drawString(2,i,menu[i]);
  }
  display.setCursor(0,0);
  display.print('>');
}

/**
 * Execute the task which matches the chosen menu item.
 */
void executeChoice(int choice) {
  switch(choice) {
      case 0 :
                display.print("Servo Angle: ");
                display.println(angle);
                display.print(" degree");
                break;
      case 1 :
                display.println("Stepper Speed: ");
                display.print(stepperSpeed);
                break;
      default :
                display.print("Error No Choice found"); 
                break;
  }
}
/* ----------------
 *  Tetsuya Tominaga
 *  Read potentio and Output Servo pulse, Display and Serial port 
 *  Board :  https://github.com/Xinyuan-LilyGO/TTGO-T-Display
 *  Sample : https://tom2rd.sakura.ne.jp/wp/2019/08/20/post-9781/
 *  Potentio :10 k ohm
 *  
 */
 //--TTGO-DISPLAY---
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <Button2.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          13
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=120, osy=120, omx=120, omy=120, ohx=120, ohy=120;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, yy0=0, yy1=0;
uint32_t targetTime = 0; 
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
int vref = 1100;
int btnCick = false;


// Include the ESP32 Arduino Servo Library instead of the original Arduino Servo Library
#include <ESP32Servo.h> 

Servo myservo;  // create servo object to control a servo

// Possible PWM GPIO pins on the ESP32: 0(used by on-board button),2,4,5(used by on-board LED),12-19,21-23,25-27,32-33 
int servoPin = 27;      // GPIO pin used to connect the servo control (digital out)
// Possible ADC pins on the ESP32: 0,2,4,12-15,32-39; 34-39 are recommended for analog input
int potPin = 12;        // GPIO pin used to connect the potentiometer (analog in)
int ADC_Max = 4095;     // This is the default ADC max value on the ESP32 (12 bit ADC width);
                        // a range of max values of 512-4096
int PWM_MAX = 2400;
int PWM_MIN = 500;  
int val;    // variable to read the value from the analog pin
int usec_val; // PWM pulse width

void setup()
{
  Serial.begin(57600); delay(10);
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(100);// Standard 100hz servo
  myservo.attach(servoPin, PWM_MIN, PWM_MAX);   // attaches the servo on pin 18 to the servo object
                                         // servo min/max of 500us and 2400us

  tft.init();

  if (TFT_BL > 0) {
        pinMode(TFT_BL, OUTPUT);
        digitalWrite(TFT_BL, HIGH);
  }
  tft.setRotation(2);
  tft.fillScreen(TFT_RED);
  delay(5000);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);                                       
}

void loop() {
  val = analogRead(potPin);            // read the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, ADC_Max, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  usec_val =map(val,0,180,PWM_MIN,PWM_MAX);
  myservo.write(val);                  // set the servo position according to the scaled value
  Serial.print(val);
  Serial.print(" % ");
  Serial.print(usec_val);
  Serial.println(" usec");

  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setRotation(1);
  tft.setCursor(0,0);
  String disp_string="";
  disp_string="Tom's Hobby";
  tft.print(disp_string);
  tft.setCursor(0,40);
  disp_string="PER :" + String(val) + "%";
  tft.print(disp_string);
  tft.setCursor(0,80);
  disp_string="PWM :" + String(usec_val) + "us";
  tft.print(disp_string);

  
  delay(20);                          // wait for the servo to get there
}

/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald

 modified for the ESP32 on March 2017
 by John Bennett
 
 see  http://www.arduino.cc/en/Tutorial/Knob for a description of the original code

 * Different servos require different pulse widths to vary servo angle, but the range is 
 * an approximately 500-2500 microsecond pulse every 20ms (50Hz). In general, hobbyist servos
 * sweep 180 degrees, so the lowest number in the published range for a particular servo
 * represents an angle of 0 degrees, the middle of the range represents 90 degrees, and the top
 * of the range represents 180 degrees. So for example, if the range is 1000us to 2000us,
 * 1000us would equal an angle of 0, 1500us would equal 90 degrees, and 2000us would equal 1800
 * degrees.
 * 
 * Circuit: (using an ESP32 Thing from Sparkfun)
 * Servo motors have three wires: power, ground, and signal. The power wire is typically red,
 * the ground wire is typically black or brown, and the signal wire is typically yellow,
 * orange or white. Since the ESP32 can supply limited current at only 3.3V, and servos draw
 * considerable power, we will connect servo power to the VBat pin of the ESP32 (located
 * near the USB connector). THIS IS ONLY APPROPRIATE FOR SMALL SERVOS. 
 * 
 * We could also connect servo power to a separate external
 * power source (as long as we connect all of the grounds (ESP32, servo, and external power).
 * In this example, we just connect ESP32 ground to servo ground. The servo signal pins
 * connect to any available GPIO pins on the ESP32 (in this example, we use pin 18.
 * 
 * In this example, we assume a Tower Pro SG90 small servo connected to VBat.
 * The published min and max for this servo are 500 and 2400, respectively.
 * These values actually drive the servos a little past 0 and 180, so
 * if you are particular, adjust the min and max values to match your needs.
 */

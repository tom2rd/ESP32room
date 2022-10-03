#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BMP085.h>
/* 
Two BMP085 
*/
#define SDA_1 21
#define SCL_1 22

#define SDA_2 16
#define SCL_2 17

TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);

Adafruit_BMP085 bmp1;
Adafruit_BMP085 bmp2;


void setup() {
  Serial.begin(9600);
  I2Cone.begin(SDA_1, SCL_1, 100000); 
  I2Ctwo.begin(SDA_2, SCL_2, 100000);
  if (!bmp1.begin(BMP085_ULTRAHIGHRES,&I2Cone)) {
	 Serial.println("Could not find a valid 1 BMP085 sensor, check wiring!");
	 while (1) {}
  }
  if (!bmp2.begin(BMP085_ULTRAHIGHRES,&I2Ctwo)) {
	 Serial.println("Could not find a valid 2 BMP085 sensor, check wiring!");
	 while (1) {}
  }
}
  
void loop() {
    Serial.print("T1 = ");
    Serial.print(bmp1.readTemperature());
    Serial.print(" *C");
    
    Serial.print(" P2 = ");
    Serial.print(bmp1.readPressure());
    Serial.print(" Pa");
    
    Serial.print(" T2 = ");
    Serial.print(bmp2.readTemperature());
    Serial.print(" *C");
    
    Serial.print(" P2 = ");
    Serial.print(bmp2.readPressure());
    Serial.println(" Pa");

    delay(100);
}

/*************************************************** 
  This is an example for the BMP085 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391

  These pressure and temperature sensors use I2C to communicate, 2 pins
  are required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

//Adafruit_BMP085 bmp;
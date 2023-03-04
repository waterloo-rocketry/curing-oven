// Calvin DeKoter
// Script to read and display the vacuum level in inches of mercury
// Using an absolute pressure sensor with 0-5V output on a resistor divider to avoid killing the Teensy Analog Pins
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

 int vacc = 14 ; // Pin number of analog sense pin
 int zero ; // Reading at zero inches of mercury
 int full = 100 ; % // Full scale reading of sensor at 29.5 inHg
 int setpoint = 28 ; // Setpoint for vacuum controller
 float vacuum = 0 ;

void setup()
{
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Group 41 Vacc");
  lcd.setCursor(2,1);
  lcd.print("Vacc Set: "+setpoint);
   lcd.setCursor(0,2);
  lcd.print("Funded by WEEF");
   lcd.setCursor(2,3);
  lcd.print("Call 226-627-9406");

  pinMode(vacc, INPUT) ;
  zero = analogRead(vacc) ;
  
}


void loop()
{

  // Read the value of the vacuum sensor

  vacuum = (analogRead(vacc) - zero) * 29.5/full ;

  delay(500) ;

  lcd.clear() ; 

  lcd.setCursor(3,0) ; 
  lcd.print("Vacuum Level: ") ;
  lcd.setCursor(7,1) ;
  lcd.print(vacuum) ;

  delay(500) ;
  

  // 
}

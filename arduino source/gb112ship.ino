 //sends data to server every 1.something minutes

//based on code from adafruit and sparkfun. pls support them!

//decide whether arduino should go to sleep 
bool sleep = true;


//dht
#include "DHT.h"
DHT dht;

//moisture
int moisturePin = 3;
int moistVal = 0;

//light
int lightPin = A0;
int lightVal = 0;

//imp
#include <SoftwareSerial.h>
#include <String.h>
SoftwareSerial mySerial(2, 3);

//sleep stuff
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
volatile int f_wdt=1;

void setup(void)
{
  mySerial.begin(19200);
  
  Serial.begin(115200);
  
  //dht
  dht.setup(10); // data pin 2
  
  delay(1000); 
  
    //sleepstuff
  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
  
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
}

void loop(void)
{
 
  Serial.println("good morning");
  
  //turn on pins for moisture and light sensor
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  
  delay(2000);
  
  //delay(dht.getMinimumSamplingPeriod());
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  //delay(1000);
  int temperature = (int) t;
  int humidity = (int) h;
  Serial.println(temperature);
  Serial.println(humidity);
  String temp = "|Temperature, " + (String)temperature + "~";
  Serial.println(temp);
  mySerial.println(temp);
  
  String humid = "|Humidity, " + (String)humidity + "~";
  Serial.println(humid);
  mySerial.println(humid);
  
  moistVal = analogRead(moisturePin);
  String moist = "|Moisture, " + (String)moistVal + "~";
  Serial.println(moist);
  mySerial.println(moist);
  
  //light
  int lightVal = analogRead(lightPin);
  Serial.println(lightVal);
  String light = "|Light, " + (String)lightVal + "~";
  Serial.println(light);
  mySerial.println(light);
  //delay(20);
  //delay(200);
  //turn off digital outputs to sensors. questionable if necessary.
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  
  //delay(1000);
  
  if (sleep == true)
  {
  //~8 seconds each * 7 = 56.
  //sleepstuff
  f_wdt=0;
  enterSleep();
  //sleepstuff
  f_wdt=0;
  enterSleep();
  //sleepstuff
  f_wdt=0;
  enterSleep();
  //sleepstuff
  f_wdt=0;
  enterSleep();
  //sleepstuff
  f_wdt=0;
  enterSleep();
  //sleepstuff
  f_wdt=0;
  enterSleep();
  //sleepstuff
  f_wdt=0;
  enterSleep();
  }
}




/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/

ISR(WDT_vect)
{
  if (sleep == true)
  {
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
  }
}

/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{
  Serial.println("goodnight");
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}


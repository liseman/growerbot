//growerbot version 1.0
//thanks to sparkfun (pcb layout libraries), adafruit (tsl2561, dht libraries), and arduino (community support, libraries). please support them all!

//garden name and growth profile
String growName = "spare3's garden";
String sourceName = "D.H. Landreth";
String sourceVariety = "yams";
String apName = "";
String urlName = "growerbot.com";

//lcd
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

//rotary encoder
int encoderPin1 = 2;
int encoderPin2 = 3;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastEncoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;
int buttonPin = 4;
bool buttonState = 0;
int menu = 0;
int menuSub = 0;
int pRotationIncrement = 25;
int nRotationIncrement = -25;
boolean menuChange = false;
boolean valueChange = false;
bool subMenu = 0;
float time = 0;
int debounce = 50;
bool previous = 0;

//light sensor
#include <Wire.h>
#include <TSL2561.h>
TSL2561 tsl(TSL2561_ADDR_FLOAT);
uint16_t ir, full, visible, luxOld, luxNew, lum;
//set target light level here
uint16_t luxGoal = 250;
//set target light proportion here
float lightProportionGoal = .7;
float lightProportion = 0;
//indicates current status of light. ? todo switch to just reading if relay pin is high or low?
bool lightOn = 0;
//mimimum and maximum amounts of time for light to turn on in seconds. only used if lightCycle = 1
bool lightCycle = 0;
int minLightCycleTime = 5;
int maxLightCycleTime = 600;
//minimum and maximum numbers of times for light to turn on per day. only used if lightDaily = 1
bool lightDaily = 0;
int minLightsDaily = 0;
int maxLightsDaily = 10;
//times light has been on today
int lightsToday = 0;
//seconds light is on
float timeLightOn = 0;
//seconds during which light level is at or above minimum lux
float timeBrightEnough = 0;
//number of seconds between turning lights off (makes sure we don't keep our assisted lighting on when it's bright enough naturally for the plants)
int lightCheckFrequency = 1200;

//temp, humidity sensor
#include <DHT.h>
#define DHTPIN 11
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float humidOld, humidNew, tempOld, tempNew;

//moisture probes
int moisturePin = 3;
int moistOld = 0, moistNew = 0;
//set target moisture level here
int moistGoal = 200;
bool pumpOn = 0;
//mimimum and maximum amounts of time for pump to turn on in seconds
int minPumpCycleTime = 5;
int maxPumpCycleTime = 600;
//length of current pump cycle
int currentCycleTime = 0;
//minimum and maximum numbers of times for pump to turn on per day
int minPumpsDaily = 0;
int maxPumpsDaily = 10;
//number of times pump has run today
int pumpsToday = 0;
//seconds pump is on
long timePumpOn = 0;
//total time elapsed
float timeTotal = 0;
float timeNew = 0;
float timeOld = 0;
int daysLeft = 30, daysElapsed = 0;

//relays
int relayWater = 12;
int relayLight = 13;

//imp 
#include <SoftwareSerial.h>
SoftwareSerial softSerial(14,15); //rx, tx

//timers
#include <Time.h>
#include <TimeAlarms.h>

//variables for input from server
boolean stringComplete = false;
String inputString = "";

//array to hold all variables we care about passing between cloud and garden
//String variablesPassing[] = {relayWater + humidNew + profileName};
String toSend;

//long resetInterval = 

void setup()
{
  inputString.reserve(200);
  
  //lcd
  lcd.begin(16, 2); 
  lcd.print("growerbot v1.0");
  
  //rotary
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  
  //light
  tsl.setGain(TSL2561_GAIN_16X);
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);
  
  //temp
  pinMode(DHTPIN, INPUT);
  dht.begin();
  
  //relays
  pinMode(relayWater, OUTPUT);
  pinMode(relayLight, OUTPUT);
  digitalWrite(relayWater, LOW);
  digitalWrite(relayLight, LOW);
  
  //define how many seconds to pass between sensor, relay checks, send data, and receive settings
  int sensorCheckFrequency = 1;
  int relayCheckFrequency = 5;
  int dataSendFrequency = 60;
  int settingReceiveFrequency = 3600;
  
  //timers
  //read sensors every 1 second
  Alarm.timerRepeat(sensorCheckFrequency, checkSensors);
  //check if relays should be on every 5 seconds, watering or lighting as necessary
  Alarm.timerRepeat(relayCheckFrequency, relayCheck);
  //send data to server every minute
  Alarm.timerRepeat(dataSendFrequency, sendData);
  //blink lights this frequency, to make sure we're not leaving them on when natural light is sufficient
  Alarm.timerRepeat(lightCheckFrequency, lightCheck);
  //todo: reset daily stuff, proportions every day
  Alarm.timerRepeat(86400, dayUpdate);
  //serial
  Serial.begin(9600);
  softSerial.begin(2400);
}

void loop()
{
  //checks state of scheduled events, as per http://answers.oreilly.com/topic/2704-how-to-create-an-arduino-alarm-that-calls-a-function/ 
  Alarm.delay(1);
  encodeDisplay();
  //check for input on softSerial
  softSerialCheck();
}

void encodeDisplay()
{
  //update display based on menu rotation
  if (encoderValue > pRotationIncrement)
  {
    if ( subMenu == 0) menu++;
    else menuSub++;
    encoderValue = 0;
    menuChange = true;
  }
  if (encoderValue < nRotationIncrement)
  {
    if ( subMenu == 0) menu--;
    else menuSub--;
    encoderValue = 0;
    menuChange = true;
  }
  
  //update lcd whenever sensor value or menu change
  if (subMenu == 0 && (menuChange == true || valueChange == true))
  {
    lcd.clear();
    lcd.setCursor(0,0);
    switch(menu) 
    {
    case -1:
      menu = 5;
      break;
    case 0:
    //grow menu
      lcd.print(growName);
      lcd.setCursor(0,1);
      lcd.print(daysLeft);
      lcd.print(" days left");
      break;
    case 1:
    //light menu
      lcd.print("light ");
      lcd.print(luxNew);
      lcd.print(" lux");
      lcd.setCursor(0,1);
      lcd.print("proportion ");
      lcd.print(lightProportion);
      break;
    case 2:
    //moisture menu
      lcd.print("moisture level");
      lcd.setCursor(0,1);
      lcd.print(moistNew);
      break;
    case 3:
    //temp/humidity
      lcd.print(tempNew);
      lcd.print(" Celsius");
      lcd.setCursor(0,1);
      lcd.print(humidNew);
      lcd.print("% humidity");
      break;
    case 4:
    //connection
      lcd.print("AP ");
      lcd.print(apName);
      lcd.setCursor(0,1);
      lcd.print(urlName);
      break;
    case 5:
      menu = 0;
      break;
    }
  }
  
  //update lcd whenever sensor value or submenu change
  if (subMenu == 1 && (menuChange == true || valueChange == true))
  {  
    lcd.clear();
    lcd.setCursor(0,0);
    //decide which submenu to show based on main menu
    switch(menu) 
    {
    case -1:
      menu = 5;
      break;
    //growMenu's submenus
    case 0:
      switch(menuSub)
      {
        case -1:
          menuSub = 1;
          break;
        case 0:
          lcd.print("planted ");
          lcd.print(daysElapsed);
          lcd.setCursor(0,1);
          lcd.print("days ago");        
          break;
        case 1: 
          lcd.print(sourceName);
          lcd.setCursor(0,1);
          lcd.print(sourceVariety);
          break;
        case 2:
          menuSub = 0;
          break;
      }
      break;
    //lightMenu's submenus
    case 1:
      switch(menuSub)
      {
        case -1:
          menuSub = 3;
          break;
        case 0:
          lcd.print("target ");
          lcd.print(luxGoal);
          lcd.print(" lux");
          lcd.setCursor(0,1);
          lcd.print("proportion ");
          lcd.print(lightProportionGoal);
          break;
        case 1: 
          lcd.print("infrared");
          lcd.setCursor(0,1);
          lcd.print(ir);
          break;
        case 2: 
          lcd.print("visible");
          lcd.setCursor(0,1);
          lcd.print(visible);          
          break;
        case 3: 
          lcd.print("lumens");
          lcd.print(lum);
          break;
        case 4: 
          menuSub = 0;
          break;
      }
      break;
    //moistMenu's submenus
    case 2:
      lcd.print("target level");
      lcd.setCursor(0,1);
      lcd.print(moistGoal);
      break;
    //tempMenu's submenus
    case 3:
      break;
    //connMenu's submenus
    case 4:  
      break;
    case 5: 
      menu = 0;
      break;
    }
  }
  
  //read button press. use debounce to adjust. thx to arduino.cc for some of code
  buttonState=digitalRead(buttonPin);
  if (buttonState == 1 && previous == 0 && millis() - time > debounce)
  {
    if (subMenu == 1) 
    {
      subMenu = 0;
      menuSub = 0;
    }
    else subMenu = 1;
    time = millis();
    menuChange == true;
  }
  previous = buttonState;
  menuChange = false;
  valueChange = false;
}

void softSerialCheck()
{
  while (softSerial.available())
  {
    char inChar = (char)softSerial.read();
    inputString += inChar; 
    //Serial.println(inputString);
    if (inChar == '\n')
    {
      stringComplete = true;
    }
  }

  if (stringComplete) 
  {
    Serial.println(inputString);
    //use light proportion setting if sent from imp cloud
    if (inputString.indexOf("L") > -1)
    {
      int start = inputString.indexOf("L") + 1; 
      int end = inputString.lastIndexOf("L");
      String lightSection = inputString.substring(start, end);
      char lightChar[] = "";
      lightSection.toCharArray(lightChar, sizeof(lightSection));
      lightProportionGoal = atof(lightChar);
    }
    //use light brightness setting if sent from imp cloud
    if (inputString.indexOf("B") > -1)
    {
      int start = inputString.indexOf("B") + 1; 
      int end = inputString.lastIndexOf("B");
      String luxSection = inputString.substring(start, end);
      char luxChar[] = "";
      luxSection.toCharArray(luxChar, sizeof(luxSection));
      luxGoal = atoi(luxChar);
      Serial.println(luxGoal);
    }
    //use moisture setting if sent from imp cloud
    if (inputString.indexOf("M") > -1)
    {
      int start = inputString.indexOf("M") + 1; 
      int end = inputString.lastIndexOf("M");
      String moistSection = inputString.substring(start, end);
      char moistChar[] = "";
      moistSection.toCharArray(moistChar, sizeof(moistSection));
      moistGoal = atoi(moistChar);
    }
    //turn on light if Ll1 is received from imp cloud. note that normal cycling still applies: may go off moments later
    if (inputString.indexOf("Ll1") > -1) digitalWrite(relayLight, HIGH);
    //turn on pump if Pp1 is received from imp cloud. note that normal cycling still applies: may go off moments later
    if (inputString.indexOf("Pp1") > -1) digitalWrite(relayWater, HIGH);
    inputString = "";
    stringComplete = false; 
  }
}

//read light, temperature, humidity, and moisture from sensors
void checkSensors()
{
  light();
  temphum();
  moist();  
  //check if any values have changed. if so, update display.
  if (luxOld != luxNew || moistOld != moistNew || humidOld != humidNew || tempOld != tempNew) valueChange = true;
}

void light()
{
  luxOld = luxNew;
  lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  visible = full - ir;
  luxNew = tsl.calculateLux(full, ir);
}

void temphum()
{
  humidOld = humidNew;
  tempOld = tempNew;
  humidNew = dht.readHumidity();
  tempNew = dht.readTemperature();
}

void moist()
{
  moistOld = moistNew;
  moistNew = analogRead(moisturePin);
  //Serial.print("moisture level ");
  //Serial.println(moistNew);
}

//read encoder turning
void updateEncoder()
{
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
  lastEncoded = encoded; //store this value for next time
}

//check whether a relay needs turned on
void relayCheck()
{
//update time elapsed in total
timeTotal = now();
timeNew = timeTotal - timeOld;
//Serial.print(timeNew);
//Serial.print(now());

//check water; turn on & increment time if below optimal level 
if (moistNew < moistGoal)
{
  Serial.println("turning water on");
  digitalWrite(relayWater, HIGH);
  //todo: off by 5 seconds on this and light; fix?
  timePumpOn = timePumpOn + timeNew;
}
else digitalWrite(relayWater, LOW);

//light
//if luxNew > luxGoal, add timeNew to timeBrightEnough
if (luxNew >= luxGoal)
{
  timeBrightEnough = timeBrightEnough + timeNew;
  //Serial.print("time counts");
  //Serial.println(timeBrightEnough);
  //add to timeLightOn if light is on
}
//calculate lightProportion
lightProportion = timeBrightEnough / timeTotal;

//turn on light if proportion < goal //and (luxNew < luxGoal or light is on)
if (lightProportion < lightProportionGoal && luxNew < luxGoal)
{
  Serial.println("turning light on");
  digitalWrite(relayLight, HIGH);
  timeLightOn = timeLightOn + timeNew;
}
//turn off light if lightroportion > lightproportiongoal
if (lightProportion >= lightProportionGoal) digitalWrite(relayLight, LOW);

timeOld = now();
}

void sendData()
{
  toSend  = String("");
  char temp[10];
  dtostrf(tempNew,1,2,temp);
  char hum[10];
  dtostrf(humidNew,1,2,hum);
  toSend = String(toSend + "L" + luxNew + "L" + "M" + moistNew + "M" + "T" + temp + "T" + "H" + hum + "H");
  //Serial.println(toSend);
  Serial.print("sending");
  Serial.println(toSend);
  softSerial.print(toSend);
}

void lightCheck()
{
    digitalWrite(relayLight, LOW);
}

void dayUpdate()
{
  daysLeft = daysLeft--;
  daysElapsed = daysElapsed++;  
}

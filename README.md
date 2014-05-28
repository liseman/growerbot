# Growerbot

Whelcome to the Growerbot github repository. The growerbot is a social gardening assistant, more details at growerbot.com

## Arduino source 

To use the current source you'll need three libraries :

* [Time Library](http://www.arduino.cc/playground/uploads/Code/Time.zip)
* [Adafruits Lux sensor library](https://github.com/adafruit/TSL2561-Arduino-Library)
* [Adafruits DHT sensor library](https://github.com/adafruit/DHT-sensor-library)

Download them and put them into the Library folder of your Aduino IDE installation.


### Linux Users

If you have installed Arduino IDE via an installer (i.e. apt-get or yum) note that your library may be in your home folder at ~/sketchbook/libraries.

Also if you have problems compiling the Time library and it's the Time/DateString.cpp file that cause the problem you can remove all the "PROGMEM" keywords to make it compile without problems.





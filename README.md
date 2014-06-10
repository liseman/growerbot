# Growerbot

Welcome to the Growerbot github repository. The growerbot is a social gardening assistant, more details at [growerbot.com](http://www.growerbot.com/)

## Arduino source 

To use the Arduino source you'll need those three libraries :

* [Time Library](http://www.arduino.cc/playground/uploads/Code/Time.zip)
* [Adafruits Lux sensor library](https://github.com/adafruit/TSL2561-Arduino-Library)
* [Adafruits DHT sensor library](https://github.com/adafruit/DHT-sensor-library)

Download them and put them into the libraries folder of your Aduino IDE installation.

To install Arduino IDE please go to : [http://arduino.cc/en/Main/Software](http://arduino.cc/en/Main/Software)

To upload the program into the Growerbot please follow the instruction on the fifth video of the [assembly videos](http://www.growerbot.com/2013/02/03/growerbot-kit-assembly-videos/).

### Linux Users

If you have installed Arduino IDE via an installer (i.e. apt-get) note that your library may be in your home folder at ~/sketchbook/libraries (Tested only with Ubuntu).
Also if you have problems compiling the Time library and it's the Time/DateString.cpp file that cause the problem you can remove all the "PROGMEM" keywords to make it compile without problems.

## Electric Imp Source

Those file are samples to use with the Electrc Imp SD card. Please refer to the Readme file into the folder for more help.


## PCBs

Those are the schematics and board design to build a Growerbot, you can also buy a kit at [www.growerbot.com/buy/](http://www.growerbot.com/buy/).

To learn what you need and how to build your Growerbot please watch the assembly videos at [www.growerbot.com/blog/](http://www.growerbot.com/2013/02/03/growerbot-kit-assembly-videos/)

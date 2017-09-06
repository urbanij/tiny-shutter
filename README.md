# Tiny shutter

A hardware device based on the ubiquitous [ESP8266](http://esp8266.net) Wi-Fi module intended to give a 
non Wi-Fi enabled DSLR the possibility to shoot remotely, via Wi-Fi,  in access point mode.
The device is supposed to be attached to the camera through a mini male jack connector-- the same way commercial devices do.

[This](https://fu0x11.github.io/tiny-shutter/) is how the UI looks like on a whatsoever browser once everything is set up.


### To see it in action:
* set an *AP_name* and *AP_pass* of your choice [here](https://github.com/fu0x11/tiny-shutter/blob/master/main.ino#L143) and [here](https://github.com/fu0x11/tiny-shutter/blob/master/main.ino#L144) respectively.
* upload `main.ino` onto the ESP8266. <sup>(1)</sup>
* connect the device you intend to use as remote shutter (e.g. your smartphone) to the Wi-Fi network *AP_name*.
* open the browser and navigate to: `192.168.4.1`.
* if no errors occur, you should be able to see [this](https://fu0x11.github.io/tiny-shutter/) interface. The connection is now established and you are ready to shoot. :sunglasses:


<!-- [This]() is the schematic of the device. -->

*/fu*

---------
<sup>(1)</sup> I used an [Arduino Uno](https://www.arduino.cc/en/main/ArduinoBoardUno) to accomplish that. Follow along [this](https://www.youtube.com/watch?v=P_ecAFpUADU) step-by-step guide in case of troubles.

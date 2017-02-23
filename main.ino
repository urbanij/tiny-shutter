/* main.ino
 *
 * Francesco Urbani
 *
 */

#include <ESP8266WiFi.h>

/* define of commands used to trigger the shutter. 
 * depending on whether I use the optocoupler or I connect the camera directly to GPIO2, these two needs to be swapped.
 */
#define TRIGGER 1   /* TRIGGER = 0, shoot occurs if GPIO2 is attached to the camera. */
                    /* TRIGGER = 1, shoot occurs if the optocoupler is involved. */
#define HALT    0

const int LED_PIN = 2;    /* LED hooked up on GPIO 2 */
const int SHOOTING_TIME = 700;   // ms. the time that the shutter button is being pressed.
                                  // in that time the camera needs to focus and shoot. 

WiFiServer server(80);
void blinkLED();

void setup() {
  initHardware();
  setupWiFi();
  server.begin();
}

void loop() {

  WiFiClient client = server.available();   /* Check if a client has connected */
  if (!client) return;

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Prepare the response. Start with the common header and change the rest according to the request:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";


  // Match the request
  int val = -1;   /* We'll use 'val' to keep track of both the request type (read/set) and value if set. */

  if (req.indexOf("/shoot") != -1)
    val = 0;
  else if (req.indexOf("/off") != -1)
    val = 1;
  else if (req.indexOf("/inter") != -1)
    val = 2;
  else if (req.indexOf("/delshoot") != -1)
    val = 3;
  else if (req.indexOf("/burst") != -1)
    val = 4;
  else    /* Otherwise the request is invalid. */
    val = 1; 

  client.flush();

  switch (val) {
    case 0: // shoot
      /* index.html */
      s += "<!DOCTYPE HTML> <html> <head> <style> h1 { font-family: arial; color: blue; text-align: center; font-size: 20px; } button { font-family: arial; box-shadow: 0 12px 36px 0 rgba(0,0,0,0.24), 0 20px 50px 0 rgba(0,0,0,0.19); width: 100%; height: 220px; /* height of a single button*/ border: none; color: white; padding: 10px; text-align: center; text-decoration: none; display: inline-block; font-size: 30px; background-color: #00DD00; } #offButton { background-color: red; } #shootButton { background-color: #00AA00; } </style> </head> <h1>Camera remote</h1> <body> <p><a href='/shoot'><button id='shootButton'>SINGLE SHOOT</button></a></p> <p><a href='/delshoot'><button>DELAYED SHOOT</button></a></p> <p><a href='/burst'><button>BURST</button></a></p> <p><a href='/inter'><button>INTERVALLOMETER</button></a></p> <p><a href='/off'><button id='offButton'>OFF</button></a></p> </body> </html> ";
      client.flush();
      client.print(s);
      val = 1;
      shoot();
      break;
    case 1: // off
      /* index.html */
      s += "<!DOCTYPE HTML> <html> <head> <style> h1 { font-family: arial; color: blue; text-align: center; font-size: 20px; } button { font-family: arial; box-shadow: 0 12px 36px 0 rgba(0,0,0,0.24), 0 20px 50px 0 rgba(0,0,0,0.19); width: 100%; height: 220px; /* height of a single button*/ border: none; color: white; padding: 10px; text-align: center; text-decoration: none; display: inline-block; font-size: 30px; background-color: #00DD00; } #offButton { background-color: red; } #shootButton { background-color: #00AA00; } </style> </head> <h1>Camera remote</h1> <body> <p><a href='/shoot'><button id='shootButton'>SINGLE SHOOT</button></a></p> <p><a href='/delshoot'><button>DELAYED SHOOT</button></a></p> <p><a href='/burst'><button>BURST</button></a></p> <p><a href='/inter'><button>INTERVALLOMETER</button></a></p> <p><a href='/off'><button id='offButton'>OFF</button></a></p> </body> </html> ";
      client.flush();
      client.print(s);
      val = 1;
      digitalWrite(LED_PIN, HALT);
      break;
    case 2: // intervallometer
      /* interv.html */
      s += "<!DOCTYPE HTML> <html> <head> <style> h1 { font-family: arial; color: blue; text-align: center; font-size: 20px; } button { font-family: arial; box-shadow: 0 12px 36px 0 rgba(0,0,0,0.24), 0 20px 50px 0 rgba(0,0,0,0.19); width: 100%; height: 1160px; /* height of a single button*/ border: none; color: white; padding: 10px; text-align: center; text-decoration: none; display: inline-block; font-size: 50px; background-color: #00DD00; } #offButton { background-color: red; } #shootButton { background-color: #00AA00; } </style> </head> <h1>Camera remote</h1> <body> <p><a href='/off'><button id='offButton'>OFF</button></a></p> </body> </html> ";
      client.flush();
      client.print(s);
      repeatIntervallometer:
      intervallometer();
      break;
    case 3: // delayed_shoot
      /* index.html */
      s += "<!DOCTYPE HTML> <html> <head> <style> h1 { font-family: arial; color: blue; text-align: center; font-size: 20px; } button { font-family: arial; box-shadow: 0 12px 36px 0 rgba(0,0,0,0.24), 0 20px 50px 0 rgba(0,0,0,0.19); width: 100%; height: 220px; /* height of a single button*/ border: none; color: white; padding: 10px; text-align: center; text-decoration: none; display: inline-block; font-size: 30px; background-color: #00DD00; } #offButton { background-color: red; } #shootButton { background-color: #00AA00; } </style> </head> <h1>Camera remote</h1> <body> <p><a href='/shoot'><button id='shootButton'>SINGLE SHOOT</button></a></p> <p><a href='/delshoot'><button>DELAYED SHOOT</button></a></p> <p><a href='/burst'><button>BURST</button></a></p> <p><a href='/inter'><button>INTERVALLOMETER</button></a></p> <p><a href='/off'><button id='offButton'>OFF</button></a></p> </body> </html> ";
      client.flush();
      client.print(s);
      val = 1;
      digitalWrite(LED_PIN, HALT);
      delay(2000);  /* ms of delay before shooting */
      shoot();
      break;
    case 4: // burst. Warning: the camera needs to be set on burst mode too!
      /* index.html */
      s += "<!DOCTYPE HTML> <html> <head> <style> h1 { font-family: arial; color: blue; text-align: center; font-size: 20px; } button { font-family: arial; box-shadow: 0 12px 36px 0 rgba(0,0,0,0.24), 0 20px 50px 0 rgba(0,0,0,0.19); width: 100%; height: 220px; /* height of a single button*/ border: none; color: white; padding: 10px; text-align: center; text-decoration: none; display: inline-block; font-size: 30px; background-color: #00DD00; } #offButton { background-color: red; } #shootButton { background-color: #00AA00; } </style> </head> <h1>Camera remote</h1> <body> <p><a href='/shoot'><button id='shootButton'>SINGLE SHOOT</button></a></p> <p><a href='/delshoot'><button>DELAYED SHOOT</button></a></p> <p><a href='/burst'><button>BURST</button></a></p> <p><a href='/inter'><button>INTERVALLOMETER</button></a></p> <p><a href='/off'><button id='offButton'>OFF</button></a></p> </body> </html> ";
      client.flush();
      client.print(s);
      val = 1;
      digitalWrite(LED_PIN, TRIGGER);
      delay(3000);
      digitalWrite(LED_PIN, HALT);
      break;
    default:
      s += "<!DOCTYPE HTML> <html> <head> <style> h1 { font-family: arial; color: blue; text-align: center; font-size: 20px; } button { font-family: arial; box-shadow: 0 12px 36px 0 rgba(0,0,0,0.24), 0 20px 50px 0 rgba(0,0,0,0.19); width: 100%; height: 220px; /* height of a single button*/ border: none; color: white; padding: 10px; text-align: center; text-decoration: none; display: inline-block; font-size: 30px; background-color: #00DD00; } #offButton { background-color: red; } #shootButton { background-color: #00AA00; } </style> </head> <h1>Camera remote</h1> <body> <p><a href='/shoot'><button id='shootButton'>SINGLE SHOOT</button></a></p> <p><a href='/delshoot'><button>DELAYED SHOOT</button></a></p> <p><a href='/burst'><button>BURST</button></a></p> <p><a href='/inter'><button>INTERVALLOMETER</button></a></p> <p><a href='/off'><button id='offButton'>OFF</button></a></p> </body> </html> ";
      client.flush();
      client.print(s);
      val = 1;
      digitalWrite(LED_PIN, HALT);
  }
 
  // client.print(s); /* Send the response to the client */
                      /* displays the actual webpage */

  Serial.println("Client disonnected");  /* The client will actually be disconnected when the function returns and 'client' object is detroyed */
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);   /* configure esp8266 as access point */
  const char AP_NameChar[] = "tiny-shutter";
  const char WiFiAPPSK[] = "423aky12";
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware() {
  // Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HALT);  // default value high, does not start shooting on startup.
  return;
}

void shoot(){
  digitalWrite(LED_PIN, TRIGGER);
  delay(SHOOTING_TIME);
  digitalWrite(LED_PIN, HALT);
  return;
}

void intervallometer(){
  while(1){
    digitalWrite(LED_PIN, HALT);
    digitalWrite(LED_PIN, TRIGGER);
    delay(SHOOTING_TIME);
    digitalWrite(LED_PIN, HALT);
    delay(5000);  // ms
  }
  return;
}


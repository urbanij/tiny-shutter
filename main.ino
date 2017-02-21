/*
  
  Francesco Urbani
  Sep 3, 2016.

  Last edit: Sep 23, 2016
  Comment: deleted "blink" LED option which was useless.

	edit feb 16, 2017
	nothing particular
*/

#include <ESP8266WiFi.h>
#define LED_PIN 2       	/* LED hooked up on GPIO 2 */

WiFiServer server(80);
void blinkLED();


void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
}


void loop() 
{

  WiFiClient client = server.available();   /* Check if a client has connected */
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1;     /* We'll use 'val' to keep track of both the */
                    /* request type (read/set) and value if set. */

  if (req.indexOf("/led/0") != -1){
    val = 0;   			/* Will write LED low */
  }
  else if (req.indexOf("/led/1") != -1){
    val = 1;        /* Will write LED high */
  }
  /* Otherwise the request is invalid. */

  if (val >= 0){
    digitalWrite(LED_PIN, val);   /* Set GPIO 2 according to the request */
  }

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";


  if(val) {
    s += "<font size = '70px'><a href=\"/led/0\">LED OFF\n\n</a><style>a{color:red;}</style></font>"; // font size seems not to work properly
    val = 1;
  }
  else if(!val) {
    s += "<font size = '80pt'><a href=\"/led/1\">LED ON\n\n</a><style>a{color:#33CC33;}</style></font>";  // font size seems not to work properly
    val = 0;
  }
  // else if(val == -2) {
  //   s += "LED is blinking\n\n";
  // }

  /* If we're setting the LED, print out a message saying that we made it. */
  if (val >= 0) {
    s += (val) ? "<font size = '330'>LED is now on</font>" : "<font size = '330'>LED is now off</font>";    // FONT DOES NOT WORK
  }
  else {
    s += "Invalid Request.<br> Try /led/1 or /led/0.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  
  delay(2);
  Serial.println("Client disonnected");

  /* The client will actually be disconnected
  	 when the function returns and 'client' object is detroyed */
}

void setupWiFi() 
{  
  WiFi.mode(WIFI_AP);
  const char AP_NameChar[] = "fuckingwificameraconnection";    /* Wi-Fi name */
  const char WiFiAPPSK[] = "sparkfun.@2+1";				       /* Wi-Fi password */
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware() 
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

/* main.ino
 *
 * Francesco Urbani
 *
 */

#include <ESP8266WiFi.h>
// #define LED_PIN 2       	/* LED hooked up on GPIO 2 */
const int LED_PIN = 2;

WiFiServer server(80);
void blinkLED();

void setup() {
  initHardware();
  setupWiFi();
  server.begin();
}

void loop() {

  WiFiClient client = server.available();   /* Check if a client has connected */
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1;   /* We'll use 'val' to keep track of both the request type (read/set) and value if set. */

  if (req.indexOf("/shoot") != -1){
    val = 0;    /* Will write LED low */
  }
  else if (req.indexOf("/off") != -1){
    val = 1;    /* Will write LED high */
  }
  else if (req.indexOf("/intervallometer") != -1){
    val = 2;
  }
  else if (req.indexOf("/delayed_shoot") != -1){
    val = 3;
  }
  else if (req.indexOf("/burst") != -1){
    val = 4;
  }
  /* Otherwise the request is invalid. */

  // if (val >= 0){  /* i.e. val == 0 or val == 1 */
  //   digitalWrite(LED_PIN, val);   /* Set GPIO 2 according to the request */
  // }
  switch (val) {
    case 0: // shoot
      digitalWrite(LED_PIN, 0);
      delay(60);
      break;
    // case 1: // off
    //   digitalWrite(LED_PIN, 1);
    //   break;
    case 2: // intervallometer
      digitalWrite(LED_PIN, 1);
      break;
    case 3: // delayed_shoot (2 seconds)
      delay(2000);
      digitalWrite(LED_PIN, 0);
      delay(60); // ? is it enough?
      // digitalWrite(LED_PIN, 1);
      break;
    case 4: // burst
      digitalWrite(LED_PIN, 0);
      delay(3000);
      // digitalWrite(LED_PIN, 1);
      break;
    default:
      digitalWrite(LED_PIN, 1);
  }

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";

  s += "<h1>Camera remote</h1>";

  if(val) {
    s += "<p><a href=\"/shoot\"><button>shoot</button></a>&nbsp</p>";
    val = 1;
  }
  else if(!val) {
    s += "<p>LED <a href=\"/off\"><button>ON</button></a>&nbsp;</p>";
    val = 0;
  }

  /* If we're setting the LED, print out a message saying that we made it. */
  // if (val >= 0) {
  //   // s += (val) ? "<font size = '330'>LED is now on</font>" : "<font size = '330'>LED is now off</font>";    // FONT DOES NOT WORK
  //   s += "";
  // }
  // else {
  //   s += "Invalid Request.<br> Try /off or /shoot.";
  // }

  s += "</html>\n";

  
  client.print(s);  /* Send the response to the client */
                    /* displays the actual webpage */
  
  delay(1);
  Serial.println("Client disonnected");  /* The client will actually be disconnected when the function returns and 'client' object is detroyed */
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);
  const char AP_NameChar[] = "fuckingwificameraconnection";    /* AP id */
  const char WiFiAPPSK[] = "sparkfun.@2+1";				             /* AP password */
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // default value high, does not start shooting on startup.
}

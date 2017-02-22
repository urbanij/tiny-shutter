/* main.ino
 *
 * Francesco Urbani
 *
 */

#include <ESP8266WiFi.h>

/* define of commands used to trigger the shutter. 
 * depending on whether I use the optocoupler or I connect the camera directly to GPIO2, these two needs to be swapped.
 */
#define TRIGGER 0		/* TRIGGER = 0, shoot occurs if GPIO2 is attached to the camera. */
										/* TRIGGER = 1, shoot occurs if the optocoupler is involved. */
#define HALT		1

const int LED_PIN = 2;    /* LED hooked up on GPIO 2 */
const int SHOOTING_TIME = 500;  // ms. the time that the shutter button is being pressed.
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
  if (!client){
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1;   /* We'll use 'val' to keep track of both the request type (read/set) and value if set. */

  if (req.indexOf("/shoot") != -1)
    val = 0;
  else if (req.indexOf("/off") != -1)
    val = 1;
  else if (req.indexOf("/intervallometer") != -1)
    val = 2;
  else if (req.indexOf("/delayed_shoot") != -1)
    val = 3;
  else if (req.indexOf("/burst") != -1)
    val = 4;
  /* Otherwise the request is invalid. */
  else {
  	val = 1;
  }

  switch (val) {
    case 0: // shoot
      digitalWrite(LED_PIN, TRIGGER);
      delay(SHOOTING_TIME);
      digitalWrite(LED_PIN, HALT);
      break;
    case 1:	// off
    	digitalWrite(LED_PIN, HALT);
    	break;
    case 2: // intervallometer
    				// configure an interrupt to make it work.
      attachInterrupt(val=2, intervallometerISR, CHANGE);
      break;
    case 3: // delayed_shoot (2 seconds)
      delay(2000);
      digitalWrite(LED_PIN, TRIGGER);
      delay(SHOOTING_TIME); // ? is it enough?
      digitalWrite(LED_PIN, HALT);
      break;
    case 4: // burst
      digitalWrite(LED_PIN, TRIGGER);
      delay(3000);
      // digitalWrite(LED_PIN, 1);
      break;
    default:
      digitalWrite(LED_PIN, HALT);
  }

  client.flush();

  // Prepare the response. Start with the common header and change the rest according to the request:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n";
  s += "<html>\r\n";

  /* adds some css-styling */
  s += "<head>" ;
	s += "<style>" ;
	// s += "body {background-color: powderblue;}" ;
	s += "h1 { color: red; text-align: center; }";
	// s += "p { color: red; }";
	s += "button { box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19); width: 100%; background-color: #4CAF50; /*green*/ border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; }";
	s += "</style>";
	s += "</head>";
	/* end styling */

  s += "<h1>Camera remote</h1>";

  if(val == 0){ /* single shoot */
    s += "<a href=\"/shoot\"><button>shoot</button></a>";
    s += "<a href=\"/intervallometer\"><button>intervallometer</button></a>";
    s += "<a href=\"/d-shoot\"><button>delayed shoot</button></a>";
    s += "<a href=\"/burst\"><button>burst</button></a>";
    s += "<a href=\"/off\"><button>off</button></a>";
    val = 1;
  }
  else if(val == 1){  /* off */ 
    s += "<a href=\"/shoot\"><button>shoot</button></a>";
    s += "<a href=\"/intervallometer\"><button>intervallometer</button></a>";
    s += "<a href=\"/d-shoot\"><button>delayed shoot</button></a>";
    s += "<a href=\"/burst\"><button>burst</button></a>";
    s += "<a href=\"/off\"><button>off</button></a>";
    val = 1;
  }
  else if(val == 2){  /* intervallometer */
    s += "<a href=\"/shoot\"><button>shoot</button></a>";
    s += "<a href=\"/intervallometer\"><button>intervallometer</button></a>";
    s += "<a href=\"/d-shoot\"><button>delayed shoot</button></a>";
    s += "<a href=\"/burst\"><button>burst</button></a>";
    s += "<a href=\"/off\"><button>off</button></a>";
    val = 1;
  }
  else if(val == 3){  /* delayed shoot */
    s += "<a href=\"/shoot\"><button>shoot</button></a>";
    s += "<a href=\"/intervallometer\"><button>intervallometer</button></a>";
    s += "<a href=\"/d-shoot\"><button>delayed shoot</button></a>";
    s += "<a href=\"/burst\"><button>burst</button></a>";
    s += "<a href=\"/off\"><button>off</button></a>";
    val = 1;
  }
  else if(val == 4){  /* burst */
    s += "<a href=\"/shoot\"><button>shoot</button></a>";
    s += "<a href=\"/intervallometer\"><button>intervallometer</button></a>";
    s += "<a href=\"/d-shoot\"><button>delayed shoot</button></a>";
    s += "<a href=\"/burst\"><button>burst</button></a>";
    s += "<a href=\"/off\"><button>off</button></a>";
    val = 1;
  }
  else {  /* off */
    s += "<a href=\"/shoot\"><button>shoot</button></a>";
    s += "<a href=\"/intervallometer\"><button>intervallometer</button></a>";
    s += "<a href=\"/d-shoot\"><button>delayed shoot</button></a>";
    s += "<a href=\"/burst\"><button>burst</button></a>";
    s += "<a href=\"/off\"><button>off</button></a>";
    val = 0;
  }
  s += "</html>\n";		/* close HTML page */

  client.print(s);  /* Send the response to the client */
                    /* displays the actual webpage */
  
  delay(1);
  Serial.println("Client disonnected");  /* The client will actually be disconnected when the function returns and 'client' object is detroyed */
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);		/* configure esp8266 as access point */
  const char AP_NameChar[]	= "AP_Name";			/* AP id */
  const char WiFiAPPSK[]		= "AP_Pass";			/* AP password */
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware() {
  // Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HALT);  // default value high, does not start shooting on startup.
}

void intervallometerISR(){
	while (1)
  {
  	// digitalWrite(LED_PIN, TRIGGER);
  	// delay(SHOOTING_TIME);
  	// digitalWrite(LED_PIN, HALT);
  	// delay(5000);	// ms
    static int state = 0;
    state = !state;
    digitalWrite(LED_PIN, state);
    delay(2000);
  }
}


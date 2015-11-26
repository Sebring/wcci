
#define ESP_RESET 9
#define GREEN 12
#define RED 10
#define YELLOW 11
#define LED 13
#define SWITCH 4

#define BLINK_COUNT 1
#define BLINK_DURATION 500

String commandLock = "GET /log_visit_state/0 HTTP/1.0\r\n\r\n";
String commandUnlock = "GET /log_visit_state/1 HTTP/1.0\r\n\r\n";

bool isOk;
int count;
int switchState;

void setup()
{
  pinMode(ESP_RESET, INPUT);
  //pinMode(ESP_RESET, OUTPUT);
  //digitalWrite(ESP_RESET, HIGH);
  
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);

/*    
  isOk = false;

*/
  
  // setup serial with esp-01
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  Serial.setTimeout(10000);

  blink(BLINK_COUNT, BLINK_DURATION, 1, 0, 0);

  if (!sendAT()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 0, 1, 0);

  if (!sendATReset()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 1, 1, 0);

  if (!sendATModeStation()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 0, 0, 1);

  if (!sendATReset()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 1, 0, 1);

  if (!sendATMultipleConnections()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 0, 1, 1);
  
  if (!sendATJoinNetwork()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 1, 1, 1);

/*


  isOk = true;
  // indicate start up
  for(uint16_t i=0; i<5; i++) {
     delay(300);
     digitalWrite(GREEN, HIGH);
     digitalWrite(YELLOW, HIGH);
     digitalWrite(RED, HIGH);
     delay(300);
     digitalWrite(GREEN, LOW);
     digitalWrite(YELLOW, LOW);
     digitalWrite(RED, LOW);  
  }
  digitalWrite(LED, LOW);
  return;

  
  //blink(); // 6

  if (!sendATConnectServer()) return;

  //blink(); // 7

  if (!sendATPrepareLockDoor()) return;

  //blink(); // 8

  if (!sendATLockDoor()) return;

  digitalWrite(LED, HIGH);
  //blink(); // 9

  if (!sendATCipClose()) return;

  digitalWrite(YELLOW, HIGH);

  delay(2000);
  digitalWrite(YELLOW, LOW);
  digitalWrite(LED, LOW);

  if (!sendATConnectServer()) return;

  if (!sendATPrepareUnlockDoor()) return;

  if (!sendATUnlockDoor()) return;
  */
   
}

void loop() 
{
  int state = digitalRead(SWITCH);
     
  if (switchState != state) {
    // send state to server
    if (state == LOW) {
      sendLock();
    } else {
      sendUnlock();
    }
  }

  switchState = state;

  if (switchState == LOW) {
    count++;
    digitalWrite(RED, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
  } else {
    if (count > 0) {
      count -= 6;
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(GREEN, LOW);
      
    } else {
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, HIGH);
    }
  }
  delay(500);
}



void blink(int count, int duration, int r, int y, int g) {
  for(uint16_t i=0; i<count; i++) {
    digitalWrite(GREEN, g);
    digitalWrite(YELLOW, y);
    digitalWrite(RED, r);
    delay(duration);

    digitalWrite(GREEN, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);
    delay(duration);
  }
}

bool sendATCipClose() {
  bool res = false;


  Serial.println("AT+CIPCLOSE");
  // listen for response
    if (Serial.find("OK")) {

      res = true;

    } else {

    }
  
  return res;
}

bool sendATUnlockDoor() {
    bool res = false;
    String url_path = "GET /log_visit_state/0 HTTP/1.0\r\n\r\n";

  Serial.println(url_path);
  // listen for response
    if (Serial.find("OK")) {

      res = true;

    } else {

    }

  
  return res;
}


bool sendATPrepareUnlockDoor() {
    bool res = false;
    String url_path = "GET /log_visit_state/0 HTTP/1.0\r\n\r\n";

  Serial.print("AT+CIPSEND=4,");
  Serial.println(url_path.length());
  // listen for response
    if (Serial.find(">")) {

      res = true;

    } else {

    }
  
  
  return res;
}


bool sendATLockDoor() {
    bool res = false;
    String url_path = "GET /log_visit_state/1 HTTP/1.0\r\n\r\n";

  Serial.println(url_path);
  // listen for response
    if (Serial.find("OK")) {

      res = true;

    } else {

    }

  
  return res;
}


bool sendATPrepareLockDoor() {
    bool res = false;
    String url_path = "GET /log_visit_state/1 HTTP/1.0\r\n\r\n";

  Serial.print("AT+CIPSEND=4,");
  Serial.println(url_path.length());
  // listen for response
    if (Serial.find(">")) {

      res = true;

    } else {

    }

  
  return res;
}

bool sendATConnectServer() {
  bool res = false;

  Serial.println("AT+CIPSTART=4,\"TCP\",\"10.225.56.58\",3000");
  // listen for response
    if (Serial.find("OK")) {

      res = true;

    } else {

    }

  
  return res;
}

bool sendATMultipleConnections() {
  
  bool res = false;
  
  Serial.println("AT+CIPMUX=1");
  

  // listen for response
    if (Serial.find("OK")) {

      res = true;

    } else {

    }
  return res;
}

bool sendATJoinNetwork() {
  bool res = false;

  Serial.println("AT+CWJAP=\"ssid\",\"pwd\"");
  // listen for response
    if (Serial.find("OK")) {

      res = true;

    } else {

    }
  
  return res;
}

bool sendATModeStation() {
  bool res = false;
  Serial.println("AT+CWMODE=3");
  

  // listen for response
    if (Serial.find("OK")) {

      res = true;

    }

  return res;
}

bool sendATReset() {
  bool res = false;
  Serial.println("AT+RST");
  
  // listen for response
    if (Serial.find("ready")) {
      res = true;
    } 
    delay(500);
    return res;
}


bool sendAT() {
  Serial.println("AT");
  if (Serial.find("OK")) {
    return true;
  }
  return false;
}

/*
void blink() {
        digitalWrite(GREEN, HIGH);
        digitalWrite(YELLOW, HIGH);
        digitalWrite(RED, HIGH);
        delay(500);
        digitalWrite(GREEN, LOW);
        digitalWrite(YELLOW, LOW);
        digitalWrite(RED, LOW);
        delay(500);
        digitalWrite(GREEN, HIGH);
        digitalWrite(YELLOW, HIGH);
        digitalWrite(RED, HIGH);
        delay(500);
        digitalWrite(GREEN, LOW);
        digitalWrite(YELLOW, LOW);
        digitalWrite(RED, LOW);
        delay(500);
}
*/

bool sendLock() {
  if (!sendATConnectServer()) return false;
  if (!sendATPrepareLockDoor()) return false;
  if (!sendATLockDoor()) return false;
  if (!sendATCipClose()) return false;
  return true;
}

bool sendUnlock() {
  if (!sendATConnectServer()) return false;
  if (!sendATPrepareUnlockDoor()) return false;
  if (!sendATUnlockDoor()) return false;
  if (!sendATCipClose()) return false;
  return true;
}

/*
void loop() {

  if (!isOk) {
    blinkForeverAndEver();
    return;
  }
  
*/

void blinkLEDForever() { 
  while(1) {
   digitalWrite(LED, HIGH);
   delay(100);
   digitalWrite(LED, LOW);
   delay(100);
  }
}

/*
void blinkForeverAndEver() { 
   digitalWrite(LED, HIGH);
   //digitalWrite(RED, HIGH);
   delay(1000);
   digitalWrite(LED, LOW);
   //digitalWrite(RED, LOW);
   delay(1000);
}
*/


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

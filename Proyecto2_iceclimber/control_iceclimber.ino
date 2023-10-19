const int arribaPin = 13; 
const int abajoPin = 12;
const int izquierdaPin = 14;
const int derechaPin = 27;
const int accionPin = 26;
const int accion2Pin = 25;

const unsigned long debounceDelay = 50; 
const unsigned long serialInterval = 100; // 1 second

int arribaState = HIGH; 
int abajoState = HIGH;
int izquierdaState = HIGH; 
int derechaState = HIGH;
int accionState = HIGH;
int accion2State = HIGH;

int lastArribaState = HIGH;
int lastAbajoState = HIGH;
int lastIzquierdaState = HIGH;
int lastDerechaState = HIGH;
int lastAccionState = HIGH;
int lastAccion2State = HIGH;

unsigned long lastDebounceTime1 = 0; 
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0; 
unsigned long lastDebounceTime4 = 0;
unsigned long lastDebounceTime5 = 0;
unsigned long lastDebounceTime6 = 0;

unsigned long lastSerialSendTime1 = 0;
unsigned long lastSerialSendTime2 = 0;
unsigned long lastSerialSendTime3 = 0;
unsigned long lastSerialSendTime4 = 0;
unsigned long lastSerialSendTime5 = 0;
unsigned long lastSerialSendTime6 = 0;

void setup() {
  pinMode(arribaPin, INPUT_PULLUP);
  pinMode(abajoPin, INPUT_PULLUP);
  pinMode(izquierdaPin, INPUT_PULLUP);
  pinMode(derechaPin, INPUT_PULLUP);
  pinMode(accionPin, INPUT_PULLUP);
  pinMode(accion2Pin, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  int reading1 = digitalRead(arribaPin);
  int reading2 = digitalRead(abajoPin);
  int reading3 = digitalRead(izquierdaPin);
  int reading4 = digitalRead(derechaPin);
  int reading5 = digitalRead(accionPin);
  int reading6 = digitalRead(accion2Pin);

  if (reading1 != lastArribaState) {
    lastDebounceTime1 = millis(); 
  }
  if (reading2 != lastAbajoState) {
    lastDebounceTime2 = millis(); 
  }
  if (reading3 != lastIzquierdaState) {
    lastDebounceTime3 = millis(); 
  }
  if (reading4 != lastDerechaState) {
    lastDebounceTime4 = millis(); 
  }
  if (reading5 != lastAccionState) {
    lastDebounceTime5 = millis(); 
  }
  if (reading6 != lastAccion2State) {
    lastDebounceTime6 = millis(); 
  }

  if ((millis() - lastDebounceTime1) > debounceDelay) {
    if (reading1 != arribaState) {
      arribaState = reading1;
      if (arribaState == LOW) {
        lastSerialSendTime1 = millis(); 
        Serial.write(10);
      } else if (lastArribaState == LOW) {
        Serial.write(01); 
      }
    }
  }
  
  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (reading2 != abajoState) {
      abajoState = reading2;
      if (abajoState == LOW) {
        lastSerialSendTime2 = millis();
        Serial.write(20);
      } else if (lastAbajoState == LOW) {
        Serial.write(02); 
      }
    }
  }
  
  if ((millis() - lastDebounceTime3) > debounceDelay) {
    if (reading3 != izquierdaState) {
      izquierdaState = reading3;
      if (izquierdaState == LOW) {
        lastSerialSendTime3 = millis(); 
        Serial.write(30);
      } else if (lastIzquierdaState == LOW) {
        Serial.write(03); 
      }
    }
  }
  
  if ((millis() - lastDebounceTime4) > debounceDelay) {
    if (reading4 != derechaState) {
      derechaState = reading4;
      if (derechaState == LOW) {
        lastSerialSendTime4 = millis(); 
        Serial.write(40);
      } else if (lastDerechaState == LOW) {
        Serial.write(04); 
      }
    }
  }

  if ((millis() - lastDebounceTime5) > debounceDelay) {
    if (reading5 != accionState) {
      accionState = reading5;
      if (accionState == LOW) {
        lastSerialSendTime5 = millis(); 
        Serial.write(50);
      } else if (lastAccionState == LOW) {
        Serial.write(05); 
      }
    }
  }

  if ((millis() - lastDebounceTime6) > debounceDelay) {
    if (reading6 != accion2State) {
      accion2State = reading6;
      if (accion2State == LOW) {
        lastSerialSendTime6 = millis(); 
        Serial.write(60);
      } else if (lastAccion2State == LOW) {
        Serial.write(06); 
      }
    }
  }

  // Chequear si volvemos a enviar un serial
  if (arribaState == LOW && (millis() - lastSerialSendTime1) >= serialInterval) {
    Serial.write(10);
    lastSerialSendTime1 = millis();
  }
  if (abajoState == LOW && (millis() - lastSerialSendTime2) >= serialInterval) {
    Serial.write(20);
    lastSerialSendTime2 = millis();
  }
  if (izquierdaState == LOW && (millis() - lastSerialSendTime3) >= serialInterval) {
    Serial.write(30);
    lastSerialSendTime3 = millis();
  }
  if (derechaState == LOW && (millis() - lastSerialSendTime4) >= serialInterval) {
    Serial.write(40);
    lastSerialSendTime4 = millis();
  }
  if (accionState == LOW && (millis() - lastSerialSendTime5) >= serialInterval) {
    Serial.write(50);
    lastSerialSendTime5 = millis();
  }
  if (accion2State == LOW && (millis() - lastSerialSendTime6) >= serialInterval) {
    Serial.write(60);
    lastSerialSendTime6 = millis();
  }

  lastArribaState = reading1;
  lastAbajoState = reading2;
  lastIzquierdaState = reading3;
  lastDerechaState = reading4;
  lastAccionState = reading5;
  lastAccion2State = reading6;
}

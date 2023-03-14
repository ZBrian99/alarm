int puertaIzq = 6;
int puertaDer = 5;
int ledIzq = 4;
int ledDer = 3;

int boton1 = 7;
bool enableLeds = true;
bool lastState=false;

void setup() {

  delay(3000);
  pinMode(puertaIzq, INPUT_PULLUP);
  pinMode(puertaDer, INPUT_PULLUP);
  pinMode(boton1, INPUT_PULLUP);
  pinMode(ledIzq, OUTPUT);
  pinMode(ledDer, OUTPUT);
  Serial.begin(9600);
  Serial.write('z');

  for (int i = 0; i < 5; i++) {
    digitalWrite(ledIzq, LOW);
    digitalWrite(ledDer, LOW);
    delay(100);
    digitalWrite(ledIzq, HIGH);
    digitalWrite(ledDer, HIGH);
    delay(100);
  }

}

void loop() {

  if (Serial.available() > 0) {
    if(Serial.read()){
    enableLeds = !enableLeds;
    if (enableLeds == false) {
        Serial.write('4');
        lastState=false;


        for (int i = 0; i < 3; i++) {

          digitalWrite(ledIzq, HIGH);
          delay(300);
          digitalWrite(ledIzq, LOW);
          delay(300);

        }

      }
      else if (enableLeds == true) {
        Serial.write('5');
        for (int i = 0; i < 3; i++) {

          digitalWrite(ledIzq, HIGH);
          delay(100);
          digitalWrite(ledIzq, LOW);
          delay(100);
        }

      }
    }
  }


  if (digitalRead(boton1) == LOW) {
    delay(100);
    if (digitalRead(boton1) == LOW) {
      enableLeds = !enableLeds;
      if (enableLeds == false) {
        Serial.write('4');
        lastState=false;

        for (int i = 0; i < 3; i++) {

          digitalWrite(ledIzq, HIGH);
          delay(300);
          digitalWrite(ledIzq, LOW);
          delay(300);

        }

      }
      else if (enableLeds == true) {
        Serial.write('5');
        for (int i = 0; i < 3; i++) {

          digitalWrite(ledIzq, HIGH);
          delay(100);
          digitalWrite(ledIzq, LOW);
          delay(100);
        }

      }

    }
  }


  //PUERTA Y LED DERECHA

  if (digitalRead(puertaDer) == LOW && digitalRead(ledDer) == HIGH) {
    delay(100);

    if (digitalRead(puertaDer) == LOW && digitalRead(ledDer) == HIGH) {
      Serial.write('2');
      digitalWrite(ledDer, LOW);

    }
  }
  if (digitalRead(puertaDer) == HIGH && digitalRead(ledDer) == LOW) {
    delay(100);

    if (digitalRead(puertaDer) == HIGH && digitalRead(ledDer) == LOW) {

      Serial.write('3');
      digitalWrite(ledDer, HIGH);

    }
  }

  if (enableLeds == false) {
    digitalWrite(ledIzq, LOW);
  if (digitalRead(puertaIzq) == LOW && lastState == true) {
      delay(100);

      if (digitalRead(puertaIzq) == LOW && lastState == true) {
        Serial.write('0');
        lastState=!lastState;
      }
    }
    if (digitalRead(puertaIzq) == HIGH && lastState == false) {
      delay(100);

      if (digitalRead(puertaIzq) == HIGH && lastState == false) {

        Serial.write('1');
        lastState=!lastState;

      }
    }
  }

  //PUERTA Y LED IZQUIERDA
  if (enableLeds == true) {

    if (digitalRead(puertaIzq) == LOW && digitalRead(ledIzq) == HIGH) {
      delay(100);

      if (digitalRead(puertaIzq) == LOW && digitalRead(ledIzq) == HIGH) {
        Serial.write('0');
        digitalWrite(ledIzq, LOW);

      }
    }
    if (digitalRead(puertaIzq) == HIGH && digitalRead(ledIzq) == LOW) {
      delay(100);

      if (digitalRead(puertaIzq) == HIGH && digitalRead(ledIzq) == LOW) {

        Serial.write('1');
        digitalWrite(ledIzq, HIGH);

      }
    }
  } 

}

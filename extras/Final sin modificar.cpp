#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "73825";
const char* password = "1029384756?SuG07";

WebSocketsServer webSocket = WebSocketsServer(81);

AsyncWebServer server(80);



// Estructura para almacenar información de dispositivos conectados
struct ConnectedDevice
{
  uint8_t num;
  String type;
};

ConnectedDevice desktop; // Almacena información sobre el escritorio
ConnectedDevice mobile;  // Almacena información sobre el móvil
ConnectedDevice web;     // Almacena información sobre la web

// Función para desconectar el dispositivo anterior del mismo tipo
void disconnectPreviousDevice(ConnectedDevice &currentDevice)
{
  if (currentDevice.num != 0)
  {
    webSocket.disconnect(currentDevice.num);
    currentDevice.num = 0;
  }
}



const int ledIzquierdoPin = 25; // Pin del LED izquierdo
const int ledDerechoPin = 26;   // Pin del LED derecho

const int puertaIzquierdaPin = 32; // Pin del sensor de la puerta izquierda (MC38)
const int puertaDerechaPin = 33;   // Pin del sensor de la puerta derecha (MC38)

bool ledIzquierdoEncendido = false;
bool ledDerechoEncendido = false;

bool comportamientoLedIzquierdoActivado = true;
bool comportamientoLedDerechoActivado = true;

const int debounceDelay = 50;        // Tiempo de debounce en milisegundos
int puertaIzquierdaPinState = HIGH;  // Estado del pin de la puerta izquierda
int puertaDerechaPinState = HIGH;    // Estado del pin de la puerta derecha

// Función para eliminar el rebote de un botón
bool debounce(int &buttonState, int buttonPin) {
  static unsigned long lastDebounceTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTime > debounceDelay) {
    int reading = digitalRead(buttonPin);

    if (reading != buttonState) {
      buttonState = reading;
      lastDebounceTime = currentTime;
      return true;
    }
  }

  return false;
}

void enviarEstadoPuertas(uint8_t num = 0) {
  String estadoPuertas = "izquierda_" + String(ledIzquierdoEncendido ? "on" : "off") +
                      "_derecha_" + String(ledDerechoEncendido ? "on" : "off");

   if (num == 0) {
    webSocket.broadcastTXT(estadoPuertas);
  } else {
    webSocket.sendTXT(num, estadoPuertas);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("Cliente " + String(num) + " se ha conectado.");
      // webSocket.sendTXT(num, "Puertas: izquierda " + String(digitalRead(puertaIzquierdaPin)) + ", derecha " + String(digitalRead(puertaDerechaPin)));

      String typeStr = String((char *)payload);

      // Verificar si el dispositivo del mismo tipo ya está conectado y desconectarlo
      if (typeStr == "desktop")
      {
        disconnectPreviousDevice(desktop);
        desktop = {num, typeStr};
      }
      else if (typeStr == "mobile")
      {
        disconnectPreviousDevice(mobile);
        mobile = {num, typeStr};
      }
      else if (typeStr == "web")
      {
        disconnectPreviousDevice(web);
        web = {num, typeStr};
      }

      enviarEstadoPuertas(num);
      break;
    case WStype_TEXT:
      if (strcmp((char*)payload, "led_izquierdo_on") == 0) {
        digitalWrite(ledIzquierdoPin, HIGH);
        ledIzquierdoEncendido = true;
      } else if (strcmp((char*)payload, "led_izquierdo_off") == 0) {
        digitalWrite(ledIzquierdoPin, LOW);
        ledIzquierdoEncendido = false;
      } else if (strcmp((char*)payload, "led_derecho_on") == 0) {
        digitalWrite(ledDerechoPin, HIGH);
        ledDerechoEncendido = true;
      } else if (strcmp((char*)payload, "led_derecho_off") == 0) {
        digitalWrite(ledDerechoPin, LOW);
        ledDerechoEncendido = false;
      } else if (strcmp((char*)payload, "activar_led_izquierdo") == 0) {
        comportamientoLedIzquierdoActivado = true;
      } else if (strcmp((char*)payload, "desactivar_led_izquierdo") == 0) {
        comportamientoLedIzquierdoActivado = false;
        digitalWrite(ledIzquierdoPin, LOW); // Apagar el LED izquierdo si se desactiva
        // ledIzquierdoEncendido = false;
      } else if (strcmp((char*)payload, "activar_led_derecho") == 0) {
        comportamientoLedDerechoActivado = true;
      } else if (strcmp((char*)payload, "desactivar_led_derecho") == 0) {
        comportamientoLedDerechoActivado = false;
        digitalWrite(ledDerechoPin, LOW); // Apagar el LED derecho si se desactiva
        // ledDerechoEncendido = false;
      }
      // enviarEstadoPuertas(num);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledIzquierdoPin, OUTPUT);
  pinMode(ledDerechoPin, OUTPUT);

  pinMode(puertaIzquierdaPin, INPUT_PULLUP);
  pinMode(puertaDerechaPin, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.print("Conexión WiFi establecida. Dirección IP: ");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin(true)) {
    Serial.println("Error al montar SPIFFS.");
    return;
  }

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  server.begin();
}

void loop() {
  webSocket.loop();

  // Monitorear el estado de la puerta izquierda y controlar el LED izquierdo
  if (digitalRead(puertaIzquierdaPin) == LOW && !ledIzquierdoEncendido) {
    if (debounce(puertaIzquierdaPinState, puertaIzquierdaPin)) {
      ledIzquierdoEncendido = true;
      enviarEstadoPuertas();
      if (comportamientoLedIzquierdoActivado) {
        digitalWrite(ledIzquierdoPin, HIGH);
      }
    }
  } else if (digitalRead(puertaIzquierdaPin) == HIGH && ledIzquierdoEncendido) {
    if (debounce(puertaIzquierdaPinState, puertaIzquierdaPin)) {
      ledIzquierdoEncendido = false;
      enviarEstadoPuertas();
      if (comportamientoLedIzquierdoActivado) {
        digitalWrite(ledIzquierdoPin, LOW);
      }
    }
  }

  // Monitorear el estado de la puerta derecha y controlar el LED derecho
  if (digitalRead(puertaDerechaPin) == LOW && !ledDerechoEncendido) {
    if (debounce(puertaDerechaPinState, puertaDerechaPin)) {
      ledDerechoEncendido = true;
      enviarEstadoPuertas();
      if (comportamientoLedDerechoActivado) {
        digitalWrite(ledDerechoPin, HIGH);
      }
    }
  } else if (digitalRead(puertaDerechaPin) == HIGH && ledDerechoEncendido) {
    if (debounce(puertaDerechaPinState, puertaDerechaPin)) {
      ledDerechoEncendido = false;
      enviarEstadoPuertas();
      if (comportamientoLedDerechoActivado) {
        digitalWrite(ledDerechoPin, LOW);
      }
    }
  }
}




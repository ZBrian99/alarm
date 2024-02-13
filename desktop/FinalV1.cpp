#include <WiFi.h>
#include <WebSocketsServer.h>

const char *ssid = "73825";
const char *password = "1029384756?SuG07";

WebSocketsServer webSocket = WebSocketsServer(81);

const uint8_t UNASSIGNED_CLIENT_NUM = -1;

// Estructura para almacenar información de dispositivos conectados
struct ConnectedDevice
{
  int id;
  int num;
  const char *type;
  bool isConnected;
};

static int ultimoId = -1;
// Incrementa y asigna el nuevo ID
// nuevoCliente.id = ++ultimoId;

ConnectedDevice desktop = {
    UNASSIGNED_CLIENT_NUM,
    UNASSIGNED_CLIENT_NUM,
    "desktop",
    false};
ConnectedDevice mobile = {
    UNASSIGNED_CLIENT_NUM,
    UNASSIGNED_CLIENT_NUM,
    "mobile",
    false};
ConnectedDevice web = {
    UNASSIGNED_CLIENT_NUM,
    UNASSIGNED_CLIENT_NUM,
    "web",
    false};

// void disconnectPreviousDevice(ConnectedDevice &currentDevice, ConnectedDevice &otherDevice)
// {
//   if (currentDevice.num != otherDevice.num && otherDevice.isConnected && currentDevice.type == otherDevice.type)
//   {
//     webSocket.disconnect(otherDevice.num);
//     otherDevice.num = UNASSIGNED_CLIENT_NUM;
//     otherDevice.isConnected = false;
//   }
// }

void handleNewConnection(uint8_t num, const String &message, ConnectedDevice &device)
// (num, "desktop", desktop);

{
  if (device.isConnected)
  {
    Serial.println("me desconectan");

    webSocket.disconnect(device.num);
    device.isConnected = false;
    // if (message == desktop.type)
    // {

    //   webSocket.disconnect(desktop.num);
    //   desktop.isConnected = false;

    //   desktop.num = num;
    //   desktop.isConnected = true;
    // }
    // else if (message == mobile.type)
    // {

    //   webSocket.disconnect(mobile.num);
    //   mobile.isConnected = false;

    //   mobile.num = num;
    //   mobile.isConnected = true;
    // }
    // else if (message == web.type)
    // {

    //   webSocket.disconnect(web.num);
    //   web.isConnected = false;

    //   web.num = num;
    //   web.isConnected = true;
    // }

    // if (currentDevice.num != otherDevice.num && otherDevice.isConnected && currentDevice.type == otherDevice.type)
    // {
    //   webSocket.disconnect(otherDevice.num);
    //   otherDevice.num = UNASSIGNED_CLIENT_NUM;
    //   otherDevice.isConnected = false;
    // }

    // Desconectar el dispositivo más antiguo del mismo tipo
    // disconnectPreviousDevice(device, desktop);
    // disconnectPreviousDevice(device, mobile);
    // disconnectPreviousDevice(device, web);
  }

  // Asignar y registrar el nuevo dispositivo
  device.id = ++ultimoId,
  device.num = num,
  device.isConnected = true;
}

bool ledIzquierdoEncendido = false;
bool ledDerechoEncendido = false;

const int ledIzquierdoPin = 25; // Pin del LED izquierdo
const int ledDerechoPin = 26;   // Pin del LED derecho

const int puertaIzquierdaPin = 32; // Pin del sensor de la puerta izquierda (MC38)
const int puertaDerechaPin = 33;   // Pin del sensor de la puerta derecha (MC38)

bool comportamientoLedIzquierdoActivado = true;
bool comportamientoLedDerechoActivado = true;

const int debounceDelay = 50;      // Tiempo de debounce en milisegundos
int puertaIzquierdaPinState = LOW; // Estado del pin de la puerta izquierda
int puertaDerechaPinState = LOW;   // Estado del pin de la puerta derecha

void ledBlink(int pin, int repeat, int interval)
{
  for (int i = 0; i < repeat; i++)
  {
    digitalWrite(pin, HIGH);
    delay(interval);
    digitalWrite(pin, LOW);
    delay(interval);
  }
}

// Función para eliminar el rebote de un botón
bool debounce(int &buttonState, int buttonPin)
{
  static unsigned long lastDebounceTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTime > debounceDelay)
  {
    int reading = digitalRead(buttonPin);

    if (reading != buttonState)
    {
      buttonState = reading;
      lastDebounceTime = currentTime;
      return true;
    }
  }

  return false;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("Cliente " + String(num) + " se ha desconectado.");

    if (desktop.num == num)
    {
      desktop.isConnected = false;
    }
    else if (mobile.num == num)
    {
      mobile.isConnected = false;
    }
    else if (web.num == num)
    {
      web.isConnected = false;
    }
    Serial.println("desconectado desde cliente");

    webSocket.disconnect(num);
    break;
  case WStype_CONNECTED:
    webSocket.broadcastTXT("");
    Serial.println("Cliente " + String(num) + " se ha conectado.");

    webSocket.sendTXT(num, "comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    webSocket.sendTXT(num, "comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));

    webSocket.sendTXT(num, "izquierda_" + String(digitalRead(puertaIzquierdaPin) ? "on" : "off"));
    webSocket.sendTXT(num, "derecha_" + String(digitalRead(puertaDerechaPin) ? "on" : "off"));
    break;
  case WStype_TEXT:
    if (strcmp((char *)payload, "ping") == 0)
    {
      String deviceType = "Desconocido";
      if (num == desktop.num)
      {
        deviceType = "Desktop";
      }
      else if (num == mobile.num)
      {
        deviceType = "Mobile";
      }
      else if (num == web.num)
      {
        deviceType = "Web";
      }

      Serial.println(String(num) + " " + deviceType + " ping");
      webSocket.sendTXT(num, "pong");
      break;
    }
    else if (strcmp((char *)payload, "desktop") == 0)
    {
      handleNewConnection(num, "desktop", desktop);
      Serial.println((char *)payload);
      break;
    }
    else if (strcmp((char *)payload, "mobile") == 0)
    {
      handleNewConnection(num, "mobile", mobile);
      Serial.println((char *)payload);
      break;
    }
    else if (strcmp((char *)payload, "web") == 0)
    {
      handleNewConnection(num, "web", web);
      Serial.println((char *)payload);
      break;
    }
    else if (strcmp((char *)payload, "led_izquierdo_on") == 0)
    {
      digitalWrite(ledIzquierdoPin, HIGH);
      ledIzquierdoEncendido = true;
      // webSocket.broadcastTXT("izquierda_on");
    }
    else if (strcmp((char *)payload, "led_izquierdo_off") == 0)
    {
      digitalWrite(ledIzquierdoPin, LOW);
      ledIzquierdoEncendido = false;
      // webSocket.broadcastTXT("izquierda_off");
    }
    else if (strcmp((char *)payload, "led_derecho_on") == 0)
    {
      digitalWrite(ledDerechoPin, HIGH);
      ledDerechoEncendido = true;
      // webSocket.broadcastTXT("derecha_on");
    }
    else if (strcmp((char *)payload, "led_derecho_off") == 0)
    {
      digitalWrite(ledDerechoPin, LOW);
      ledDerechoEncendido = false;
      // webSocket.broadcastTXT("derecha_off");
    }
    else if (strcmp((char *)payload, "activar_led_izquierdo") == 0)
    {
      comportamientoLedIzquierdoActivado = true;
      digitalWrite(ledIzquierdoPin, ledIzquierdoEncendido);
      webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "desactivar_led_izquierdo") == 0)
    {
      comportamientoLedIzquierdoActivado = false;
      digitalWrite(ledIzquierdoPin, ledIzquierdoEncendido);
      webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "activar_led_derecho") == 0)
    {
      comportamientoLedDerechoActivado = true;
      digitalWrite(ledDerechoPin, ledDerechoEncendido);
      webSocket.broadcastTXT("comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "activar_desactivar_led_izquierdo") == 0)
    {
      comportamientoLedIzquierdoActivado = !comportamientoLedIzquierdoActivado;

      if (comportamientoLedIzquierdoActivado)
      {
        ledBlink(ledIzquierdoPin, 3, 150);
        digitalWrite(ledIzquierdoPin, ledIzquierdoEncendido);
      }
      else
      {
        ledBlink(ledIzquierdoPin, 3, 300);
        digitalWrite(ledIzquierdoPin, LOW);
      }
      webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "activar_desactivar_led_derecho") == 0)
    {
      comportamientoLedDerechoActivado = !comportamientoLedDerechoActivado;

      if (comportamientoLedDerechoActivado)
      {
        ledBlink(ledDerechoPin, 3, 150);

        digitalWrite(ledDerechoPin, ledDerechoEncendido);
      }
      else
      {
        ledBlink(ledDerechoPin, 3, 300);
        digitalWrite(ledDerechoPin, LOW);
      }
      webSocket.broadcastTXT("comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "desactivar_led_derecho") == 0)
    {
      comportamientoLedDerechoActivado = false;
      digitalWrite(ledDerechoPin, ledDerechoEncendido);
      webSocket.sendTXT(num, "comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "obtener_comportamiento_izquierda") == 0)
    {
      webSocket.sendTXT(num, "comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "obtener_comportamiento_derecha") == 0)
    {
      webSocket.sendTXT(num, "comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "obtener_puerta_izquierda") == 0)
    {
      webSocket.sendTXT(num, "izquierda_" + String(digitalRead(puertaIzquierdaPin) ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "obtener_puerta_derecha") == 0)
    {
      webSocket.sendTXT(num, "derecha_" + String(digitalRead(puertaDerechaPin) ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "obtener_estado_puertas") == 0)
    {
      webSocket.broadcastTXT("izquierda_" + String(digitalRead(puertaIzquierdaPin) ? "on" : "off"));
      webSocket.broadcastTXT("derecha_" + String(digitalRead(puertaDerechaPin) ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "check") == 0)
    {
      webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
      webSocket.broadcastTXT("comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
      webSocket.broadcastTXT("izquierda_" + String(digitalRead(puertaIzquierdaPin) ? "on" : "off"));
      webSocket.broadcastTXT("derecha_" + String(digitalRead(puertaDerechaPin) ? "on" : "off"));
    }
    Serial.println((char *)payload);
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(ledIzquierdoPin, OUTPUT);
  pinMode(ledDerechoPin, OUTPUT);
  pinMode(puertaIzquierdaPin, INPUT_PULLUP);
  pinMode(puertaDerechaPin, INPUT_PULLUP);

  digitalWrite(ledIzquierdoPin, HIGH);
  digitalWrite(ledDerechoPin, HIGH);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Conectando a WiFi...");
  }

  digitalWrite(ledIzquierdoPin, LOW);
  digitalWrite(ledDerechoPin, LOW);
  delay(150);
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(ledIzquierdoPin, HIGH);
    digitalWrite(ledDerechoPin, HIGH);
    delay(150);
    digitalWrite(ledIzquierdoPin, LOW);
    digitalWrite(ledDerechoPin, LOW);
    delay(150);
  }

  Serial.print("Conexión WiFi establecida. Dirección IP: ");
  Serial.println(WiFi.localIP());

  // digitalWrite(puertaIzquierdaPinState, digitalRead(puertaIzquierdaPin));
  // digitalWrite(puertaDerechaPinState, digitalRead(puertaDerechaPin));

  // Serial.println(digitalRead(puertaIzquierdaPin));
  // Serial.println(digitalRead(puertaIzquierdaPinState));
  // Serial.println(digitalRead(puertaDerechaPin));
  // Serial.println(digitalRead(puertaDerechaPinState));

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

// puertaIzquierdaPin == LOW
// digitalWrite(ledIzquierdoPin, LOW);

// puertaIzquierdaPin == HIGH
// digitalWrite(ledIzquierdoPin, HIGH);

void loop()
{

  // Monitorear el estado de la puerta izquierda y controlar el LED izquierdo
  if (digitalRead(puertaIzquierdaPin) == LOW && ledIzquierdoEncendido)
  {
    // Serial.println("puertaIzquierdaPin == LOW");
    if (debounce(puertaIzquierdaPinState, puertaIzquierdaPin))
    {

      if (comportamientoLedIzquierdoActivado)
      {
        digitalWrite(ledIzquierdoPin, LOW);
      }
      ledIzquierdoEncendido = false;
      webSocket.broadcastTXT("izquierda_" + String(!ledIzquierdoEncendido ? "on" : "off"));
      // else
      // {
      //   webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
      // }
    }
  }
  else if (digitalRead(puertaIzquierdaPin) == HIGH && !ledIzquierdoEncendido)

  {
    // Serial.println("puertaIzquierdaPin == HIGH");
    if (debounce(puertaIzquierdaPinState, puertaIzquierdaPin))
    {
      if (comportamientoLedIzquierdoActivado)
      {
        digitalWrite(ledIzquierdoPin, HIGH);
      }
      ledIzquierdoEncendido = true;
      webSocket.broadcastTXT("izquierda_" + String(!ledIzquierdoEncendido ? "on" : "off"));
      // else
      // {
      //   webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
      // }
    }
  }

  // Monitorear el estado de la puerta derecha y controlar el LED derecho
  if (digitalRead(puertaDerechaPin) == LOW && ledDerechoEncendido)
  {
    if (debounce(puertaDerechaPinState, puertaDerechaPin))
    {
      if (comportamientoLedDerechoActivado)
      {
        digitalWrite(ledDerechoPin, LOW);
      }
      ledDerechoEncendido = false;
      webSocket.broadcastTXT("derecha_" + String(!ledDerechoEncendido ? "on" : "off"));
      // else
      // {
      //   webSocket.broadcastTXT("comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
      // }
    }
  }
  else if (digitalRead(puertaDerechaPin) == HIGH && !ledDerechoEncendido)
  {
    if (debounce(puertaDerechaPinState, puertaDerechaPin))
    {
      if (comportamientoLedDerechoActivado)
      {
        digitalWrite(ledDerechoPin, HIGH);
      }
      ledDerechoEncendido = true;
      webSocket.broadcastTXT("derecha_" + String(!ledDerechoEncendido ? "on" : "off"));
      // else
      // {
      //   webSocket.broadcastTXT("comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
      // }
    }
  }
  webSocket.loop();
}
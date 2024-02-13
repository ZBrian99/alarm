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

const int ledIzquierdoPin = 25;
const int ledDerechoPin = 26;

bool ledIzquierdoEncendido = false;
bool ledDerechoEncendido = false;

const int debounceDelay = 50;
int puertaIzquierdaPin = false;
int puertaDerechaPin = false;

bool comportamientoLedIzquierdoActivado = true;
bool comportamientoLedDerechoActivado = true;

bool debounce(bool buttonState, bool buttonPin)
{
  static unsigned long lastDebounceTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTime > debounceDelay)
  {
    bool reading = buttonPin;

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
    Serial.println("me desconecto yo");

    webSocket.disconnect(num);
    break;
  case WStype_CONNECTED:
    webSocket.broadcastTXT("");
    Serial.println("Cliente " + String(num) + " se ha conectado.");

    webSocket.sendTXT(num, "comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    webSocket.sendTXT(num, "comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));

    webSocket.sendTXT(num, "izquierda_" + String(ledIzquierdoEncendido ? "on" : "off"));
    webSocket.sendTXT(num, "derecha_" + String(ledDerechoEncendido ? "on" : "off"));
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
      webSocket.broadcastTXT("izquierda_on");
    }
    else if (strcmp((char *)payload, "led_izquierdo_off") == 0)
    {
      digitalWrite(ledIzquierdoPin, LOW);
      ledIzquierdoEncendido = false;
      webSocket.broadcastTXT("izquierda_off");
    }
    else if (strcmp((char *)payload, "led_derecho_on") == 0)
    {
      digitalWrite(ledDerechoPin, HIGH);
      ledDerechoEncendido = true;
      webSocket.broadcastTXT("derecha_on");
    }
    else if (strcmp((char *)payload, "led_derecho_off") == 0)
    {
      digitalWrite(ledDerechoPin, LOW);
      ledDerechoEncendido = false;
      webSocket.broadcastTXT("derecha_off");
    }
    else if (strcmp((char *)payload, "activar_led_izquierdo") == 0)
    {
      comportamientoLedIzquierdoActivado = true;
      digitalWrite(ledIzquierdoPin, HIGH);
      webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "desactivar_led_izquierdo") == 0)
    {
      comportamientoLedIzquierdoActivado = false;
      digitalWrite(ledIzquierdoPin, LOW);
      webSocket.broadcastTXT("comportamiento_izquierda_" + String(comportamientoLedIzquierdoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "activar_led_derecho") == 0)
    {
      comportamientoLedDerechoActivado = true;
      digitalWrite(ledDerechoPin, HIGH);
      webSocket.broadcastTXT("comportamiento_derecha_" + String(comportamientoLedDerechoActivado ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "desactivar_led_derecho") == 0)
    {
      comportamientoLedDerechoActivado = false;
      digitalWrite(ledDerechoPin, LOW);
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
      webSocket.sendTXT(num, "izquierda_" + String(ledIzquierdoEncendido ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "obtener_puerta_derecha") == 0)
    {
      webSocket.sendTXT(num, "derecha_" + String(ledDerechoEncendido ? "on" : "off"));
    }
    else if (strcmp((char *)payload, "obtener_estado_puertas") == 0)
    {
      webSocket.broadcastTXT("izquierda_" + String(ledIzquierdoEncendido ? "on" : "off"));
      webSocket.broadcastTXT("derecha_" + String(ledDerechoEncendido ? "on" : "off"));
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

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(ledIzquierdoPin, LOW);
    digitalWrite(ledDerechoPin, LOW);
    delay(1000);
    digitalWrite(ledIzquierdoPin, HIGH);
    digitalWrite(ledDerechoPin, HIGH);
    Serial.println("Conectando a WiFi...");
  }

  Serial.print("Conexión WiFi establecida. Dirección IP: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  for (int i = 0; i < 3; i++)
  {
    digitalWrite(ledIzquierdoPin, LOW);
    delay(100);
    digitalWrite(ledDerechoPin, HIGH);
    delay(100);
    digitalWrite(ledIzquierdoPin, HIGH);
    delay(100);
    digitalWrite(ledDerechoPin, LOW);
    delay(100);
  }
}

void loop()
{
  webSocket.loop();
}
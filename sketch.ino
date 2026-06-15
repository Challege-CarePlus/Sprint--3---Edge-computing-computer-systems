#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// MQTT 
const char* mqtt_server = "34.28.23.230";
const int mqtt_port = 1883;
const char* apikey = "4jggokgpepnvsb2uv4s40d59ov";
const char* device_id = "sensor001";

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pinos
#define PIN_BPM    34
#define PIN_SPO2   35
#define PIN_TEMP   32
#define PIN_LED    33 
#define PIN_BUZZER 25 

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Configurações de Saída (LED e Buzzer)
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER); 

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED não encontrado");
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  Serial.print("Conectando WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  client.setServer(mqtt_server, mqtt_port);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    if (client.connect("ESP32_CarePlus")) {
      Serial.println("conectado!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Leitura SIMPLES e direta (como na sua versão original)
  int rawBPM  = analogRead(PIN_BPM);
  int rawSPO2 = analogRead(PIN_SPO2);
  int rawTEMP = analogRead(PIN_TEMP);

  int bpm;
  int spo2;
  float temp;

  // Lógica de Zonas Realistas usando if simples
  if (rawBPM < 1365) {
    bpm = map(rawBPM, 0, 1365, 50, 70);       
  } else if (rawBPM < 2730) {
    bpm = map(rawBPM, 1366, 2730, 71, 100);   
  } else {
    bpm = map(rawBPM, 2731, 4095, 101, 180);  
  }

  if (rawSPO2 < 1000) {
    spo2 = map(rawSPO2, 0, 1000, 80, 89);     
  } else if (rawSPO2 < 2000) {
    spo2 = map(rawSPO2, 1001, 2000, 90, 94);  
  } else {
    spo2 = map(rawSPO2, 2001, 4095, 95, 100); 
  }

  if (rawTEMP < 1000) {
    temp = map(rawTEMP, 0, 1000, 340, 359) / 10.0;     
  } else if (rawTEMP < 3000) {
    temp = map(rawTEMP, 1001, 3000, 360, 375) / 10.0;  
  } else {
    temp = map(rawTEMP, 3001, 4095, 376, 410) / 10.0;  
  }

  // Lógica de Alerta manual e direta
  int alerta = 0;

  if (bpm < 60) {
    alerta = 1;
  }
  if (bpm > 100) {
    alerta = 1;
  }
  if (spo2 < 95) {
    alerta = 1;
  }
  if (temp < 36.0) {
    alerta = 1;
  }
  if (temp > 37.5) {
    alerta = 1;
  }

  if (alerta == 1) {
    digitalWrite(PIN_LED, HIGH); 
    tone(PIN_BUZZER, 1000);      
  } else {
    digitalWrite(PIN_LED, LOW);  
    noTone(PIN_BUZZER);          
  }

  // Prepara o Payload
  char payload[64];
  snprintf(payload, sizeof(payload),
    "bpm|%d|spo2|%d|t|%.1f",
    bpm, spo2, temp);

  // Tópico FIWARE
  char topic[80];
  snprintf(topic, sizeof(topic),
    "/%s/%s/attrs", apikey, device_id);

  // Publica e Imprime no terminal
  client.publish(topic, payload);
  Serial.print("Publicado: ");
  Serial.println(payload);

  // Atualiza a Tela OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Care Plus - Health");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 14);
  display.print("BPM: ");
  display.println(bpm);

  display.setTextSize(1);
  display.setCursor(0, 38);
  display.print("SpO2: ");
  display.print(spo2);
  display.println(" %");

  display.setCursor(0, 50);
  display.print("Temp: ");
  display.print(temp, 1);
  display.println(" C");

  display.display();

  // Pausa simples de 3 segundos
  delay(3000);
}

// Sketch desenvolvido para monitoramento de sensores e acionamento de um ventilador para aeração via IoT
// Laboratório de Agrotecnologia UFPEL e Laboratório de Instrumentação Agrícola UFPEL 
// Engenheiro Agrícola e Mestrando em PPGCA da UFPEL Andrigo Farias Xavier
// Criação: 12/06/2024

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClient.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Configurações da rede Wi-Fi
const char* ssid = "OsirMax_Andrigo"; // Nome da rede de Wi-Fi utilizada
const char* password = "04091981";    // Senha da rede de Wi-Fi utilizada

// Configurações do servidor MQTT (Adafruit IO)
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    "andrigoxavier"
#define AIO_KEY         "aio_ORUF51yeyJwsfzDXbKNn9C2dcsOs"

// Pinos dos sensores DS18B20
const int sensor1Pin = D1; // Conecte o primeiro sensor aqui
const int sensor2Pin = D2; // Conecte o segundo sensor aqui
const int sensor3Pin = D3; // Conecte o terceiro sensor aqui
const int sensor4Pin = D4; // Conecte o quarto sensor aqui
const int ledPin = D5;     // Pino do LED

OneWire oneWire1(sensor1Pin);
OneWire oneWire2(sensor2Pin);
OneWire oneWire3(sensor3Pin);
OneWire oneWire4(sensor4Pin);

DallasTemperature sensor1(&oneWire1);   //utilizar o mesmo nome do feed relacionado a este sensor no site da adafruit
DallasTemperature sensor2(&oneWire2);   //utilizar o mesmo nome do feed relacionado a este sensor no site da adafruit
DallasTemperature sensor3(&oneWire3);   //utilizar o mesmo nome do feed relacionado a este sensor no site da adafruit
DallasTemperature sensor4(&oneWire4);   //utilizar o mesmo nome do feed relacionado a este sensor no site da adafruit

// Configurações do cliente MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Tópicos MQTT para cada sensor e para o LED
Adafruit_MQTT_Publish temp1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor1"); 
Adafruit_MQTT_Publish temp2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor2");
Adafruit_MQTT_Publish temp3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor3");
Adafruit_MQTT_Publish temp4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor4");
Adafruit_MQTT_Publish ledFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ventilador");

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  mqtt.connect();
  while (!mqtt.connected()) {
    delay(1000);
    Serial.println("Connecting to MQTT...");
  }
  Serial.println("Connected to MQTT");

  pinMode(ledPin, OUTPUT); // Configura o pino do LED como saída 
}

void loop() {
  sensor1.requestTemperatures();
  sensor2.requestTemperatures();
  sensor3.requestTemperatures();
  sensor4.requestTemperatures();

  float temp1Value = sensor1.getTempCByIndex(0);
  float temp2Value = sensor2.getTempCByIndex(0);
  float temp3Value = sensor3.getTempCByIndex(0);
  float temp4Value = sensor4.getTempCByIndex(0);

  temp1.publish(temp1Value);
  temp2.publish(temp2Value);
  temp3.publish(temp3Value);
  temp4.publish(temp4Value);

  // Verifica se o LED deve ser ligado ou desligado
  if (digitalRead(ledPin) == HIGH) {
    ledFeed.publish("ON");
  } else {
    ledFeed.publish("OFF");
  }

  delay(60000); // Envia as leituras a cada 1 minuto
}

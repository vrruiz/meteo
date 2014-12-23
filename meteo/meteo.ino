#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"

// Optimizacion: comentar la linea de abajo para evitar las llamadas a Serial
#define DEBUG
#define PAUSA_S 60
#define SS_PIN 10

#define DHTPIN 2 
#define DHTTYPE DHT22 

RTC_DS1307 rtc; // Reloj
DHT dht(DHTPIN, DHTTYPE); // Sensor temperatura y humedad

// Inicializacion del sistema
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  Wire.begin();

  // Inicializacion del reloj RTC
  rtc.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Establecer hora
  if (!rtc.isrunning()) {
    // Problema leyendo el reloj
#ifdef DEBUG
    Serial.println("RTC is NOT running");
#endif
    return;
  }

  // Inicializacion de la tarjeta SD
  pinMode(SS_PIN, OUTPUT);
  if (!SD.begin(SS_PIN)) {
    // Problema en la inicializacion de la tarjeta SD
#ifdef DEBUG
    Serial.println("SD is NOT working");
#endif
    return;
  }
  
  // Inicializacion del sensor de temperatura
  dht.begin();
  
  // Indicar cabecera
  guardar(String("#Fecha,Temperatura,Humedad"));
}

// Bucle principal
void loop() {
  String linea;
  
  // Obtener hora
  DateTime now = rtc.now();
  String fecha;
  fecha = String(now.day()) + String("/") +
          String(now.month()) + String("/") +
          String(now.year()) + String(" ") +
          String(now.hour()) + String(":") +
          String(now.minute());
  linea = fecha;
  
  // Obtener temperatura
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  linea = linea + "," + String(temp) + "," + String(hum);

  // Guardar datos
  guardar(linea);

  // Pausa
  delay(PAUSA_S * 1000);
}

void guardar(String const &linea) {
  // Guardar datos
  File csv = SD.open("datalog.csv", FILE_WRITE);
  if (csv) {
    csv.println(linea);
    csv.close();
#ifdef DEBUG
    Serial.println(linea);
#endif
  } else {
#ifdef DEBUG
    Serial.println("Problema con la tarjeta");
#endif
  }
}


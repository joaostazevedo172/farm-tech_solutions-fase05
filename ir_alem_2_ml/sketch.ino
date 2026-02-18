/*
  🚜 FarmTech Solutions - Opção 2: Machine Learning
  Sistema: Coleta de Dados para Classificação de Saúde da Planta
  Hardware: ESP32 + DHT22 (Pino 15)
*/

#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("--- SISTEMA DE MONITORAMENTO FARMTECH ---");
  Serial.println("Aguardando leituras do sensor...");
}

void loop() {
  delay(2000); // Aguarda 2 segundos entre leituras

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Erro ao ler o sensor DHT!");
    return;
  }

  // Formato padronizado para facilitar a leitura
  Serial.print("LEITURA >> Temp: ");
  Serial.print(t);
  Serial.print(" C | Umidade: ");
  Serial.print(h);
  Serial.println(" %");
}
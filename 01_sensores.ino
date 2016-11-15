#include "DHT.h"

#define  DHTPIN D2  // pino ligado ao sensor DHT (umid e temp do ar)
#define  DHTTYPE DHT11  //modelo do sensor, DHT11.
#define  UMIDPIN D1  //pino sensor umidade de solo
#define  LDRPIN A0  //pino sensor de luminosidade

DHT dht(DHTPIN, DHTTYPE); //define porta e tipo do sensor a ser usado pela biblioteca.

//Variáveis globais, usadas para atualizar ThingSpeak
int umid_solo = 0;
int umid_ar = 0;
int temp = 0;
int lumin = 0;

void setup() {
  dht.begin(); //inicia sensor DHT11
  pinMode(LED_BUILTIN, OUTPUT); //define pino do LED como saida
  pinMode (DHTPIN, INPUT);
  pinMode(UMIDPIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  ler_umidade_do_solo();
  Serial.print(" | ");

  ler_temperatura_e_umidade_do_ar();
  Serial.print(" | ");

  ler_luminosidade();
  Serial.print(" | ");

  Serial.println(""); //pula linha no serial monitor
}

void ler_umidade_do_solo() {
  int umidade = digitalRead(UMIDPIN); //lê o valor da entrada analógica
  umid_solo = umidade;
  delay(300);
  Serial.print("Umidade solo: ");
  Serial.print(umidade); // 1 = seco; 0 = umido.
}

void ler_temperatura_e_umidade_do_ar() {
  float umidade = dht.readHumidity();
  umid_ar = umidade;

  float temperatura = dht.readTemperature();
  temp = temperatura;

  delay(300);
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.print("Falhou ao ler do sensor DHT");
  } else {
    Serial.print(" Umidade: ");
    Serial.print(umidade);
    Serial.print(" %");
    Serial.print(" | ");
    Serial.print(" Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" *C");
  }
}

void ler_luminosidade() {
  float leitura_lumi = analogRead(LDRPIN);
  float lumi = map(leitura_lumi, 1023, 0, 0, 1000); //conversao para lux aproximada
  lumin = lumi;
  Serial.print(" Luminosidade: ");
  Serial.print(lumi);
  Serial.print(" lux");
}

#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#define DHTTYPE DHT11 // Modelo do sensor (DHT11).
#define UMIDPIN D1    // Pino do sensor umidade de solo.
#define DHTPIN  D2    // Pino do sensor DHT11 (temperatura e umidade do ar).
#define LDRPIN  A0    // Pino do sensor LDR (luminosidade).

// Configuracao da rede WiFi.
const int HTTP_PORT= 80;
const char* SSID = "<SSID>"; // ATUALIZAR
const char* PASSWORD = "<PASSWORD>"; // ATUALIZAR

// Configuracao do ThingSpeak.
const char* TS_HOST = "api.thingspeak.com";
const char* TS_URL = "/update";
const char* TS_API_KEY = "<TS_API_KEY>"; // ATUALIZAR

// Configuracao do IFTTT.
const char* IFTTT_HOST = "maker.ifttt.com";
const char* IFTTT_URL = "/trigger/<EVENT>/with/key/"; // ATUALIZAR
const char* IFTTT_API_KEY = "<IFTTT_API_KEY>"; // ATUALIZAR

// Variaveis globais, usadas para atualizar ThingSpeak e enviar alerta para
// IFTTT.
int umid_solo = 0;
int umid_ar = 0;
int temp = 0;
int lumin = 0;

ESP8266WiFiMulti wifi;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Inicia sensor DHT11.
  dht.begin();

  // Configura WiFi.
  wifi.addAP(SSID, PASSWORD);

  // Define pinos de entrada e saida.
  pinMode(LED_BUILTIN, OUTPUT); // Pino de entrada do LED.
  pinMode(DHTPIN, INPUT); // Pino de saida do DHT11.
  pinMode(UMIDPIN, INPUT); // Pino de saida do sensor de umidade de solo.

  // Inicializa monitor serial (para analisarmos as saidas do programa na IDE).
  Serial.begin(115200);
  Serial.println("Setup finalizado");
}

void loop() {
  // Le sensor de umidade do solo.
  umidade_solo();
  Serial.print(" | ");

  // Le sensor de temperature e umidade do ar.
  temp_umid(); //leitura sensor temp e umid AR
  Serial.print(" | ");

  // Le sensor de luminosidade.
  luminosidade();
  Serial.println(" | ");

  // Se a WiFi esta devidamente configurada e conectada.
  if (wifi.run() == WL_CONNECTED) {
    // Envia dados para o ThingSpeak e espera um periodo curto de tempo.
    updateThingSpeak();
    delay(500);

    // Se a umidade do solo estiver baixa (ou seja, o solo esta seco).
    if (umid_solo == 0) {
      // Envia alerta para IFTTT e espera um periodo curto de tempo
      alertIFTTT();
      delay(300);
    }
  }

  // Espera 3seg para executar o proximo ciclo.
  delay(3000);
}

// Le umidade do solo do sensor conectado ao pino UMIDPIN e atualiza a variavel
// global.
void umidade_solo() {
  int umid = digitalRead(UMIDPIN); // Le o valor da entrada analogica.
  umid_solo = umid; // Variavel global para atualizar ThingSpeak.
  delay(300);

  Serial.print("Umidade do solo: ");
  Serial.print(umid); // 1 = seco; 0 = umido.
}

// Le temperatura e umidade do ar do sensor utilizando a biblioteca do DHT e
// atualizando a variavel global.
void temp_umid() {
  float h = dht.readHumidity();
  umid_ar = h; // Variavel global para atualizar ThingSpeak.
  float t = dht.readTemperature();
  temp = t; // Variavel global para atualizar ThingSpeak.
  delay(300);

  if (isnan(t) || isnan(h)) {
    Serial.print("Falha ao tentar ler o DHT");
  } else {
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.print(" %");
    Serial.print(" | ");
    Serial.print(" Temperatura: ");
    Serial.print(t);
    Serial.print(" *C");
  }
}
// Le luminosidade do sensor conectado ao pino LDRPIN atualiza a variabel global.
void luminosidade(){
  float leitura_lumi = analogRead(LDRPIN); // Le o valor fornecido pelo LDR.
  float lumi = map(leitura_lumi, 1023, 0, 0, 1000); // Conversao para lux (aproximada).
  lumin = lumi; // Variavel global para atualizar ThingSpeak.
  Serial.print(" Luminosidade: ");
  Serial.print(lumi);
  Serial.print(" lux");
}

// Envia alerta para o IFTTT.
void alertIFTTT() {
  HTTPClient client;
  String url = String(IFTTT_URL) + IFTTT_API_KEY;

  client.begin(IFTTT_HOST, HTTP_PORT, url);
  int httpCode = client.GET();

  Serial.println(String("IFTTT retornou HTTP CODE ") + httpCode);
}

// Envia dados para ThingSpeak.
void updateThingSpeak() {
  HTTPClient client;
  String url = String(TS_URL) +
    "?api_key=" + TS_API_KEY +
    "&field1=" + temp +
    "&field2=" + umid_ar +
    "&field3=" + lumin;

  client.begin(TS_HOST, HTTP_PORT, url);
  int httpCode = client.GET();
  client.end();

  Serial.println(String("ThingSpeak retornou HTTP CODE ") + httpCode);
}

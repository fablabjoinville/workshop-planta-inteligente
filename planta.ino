#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#define DHTTYPE DHT11 // Modelo do sensor (DHT11).
#define UMIDPIN D1    // Pino do sensor umidade de solo.
#define DHTPIN  D2    // Pino do sensor DHT11 (temperatura e umidade do ar).
#define LDRPIN  A0    // Pino do sensor LDR (luminosidade).

// Configuracao da rede WiFi.
const int HTTP_PORT= 80;
const char* NOME_REDE = "<NOME_REDE>"; // ATUALIZAR
const char* SENHA = "<SENHA>"; // ATUALIZAR

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
int umidSolo = 0;
int umidAr = 0;
int temp = 0;
int lumin = 0;

ESP8266WiFiMulti wifi;
DHT dht(DHTPIN, DHTTYPE);

// Le umidade do solo do sensor conectado ao pino UMIDPIN e atualiza a variavel
// global.
void leUmidadeDoSolo() {
  int umid = digitalRead(UMIDPIN); // Le o valor da entrada analogica.
  umidSolo = umid; // Variavel global para atualizar ThingSpeak.
  delay(300);

  Serial.print("Umidade do solo: ");
  Serial.print(umid); // 1 = seco; 0 = umido.
}

// Le temperatura e umidade do ar do sensor utilizando a biblioteca do DHT e
// atualizando a variavel global.
void leTemperaturaEUmidadeDoAr() {
  float h = dht.readHumidity();
  umidAr = h; // Variavel global para atualizar ThingSpeak.
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
void leLuminosidade(){
  float leitura_lumi = analogRead(LDRPIN); // Le o valor fornecido pelo LDR.
  float lumi = map(leitura_lumi, 1023, 0, 0, 1000); // Conversao para lux (aproximada).
  lumin = lumi; // Variavel global para atualizar ThingSpeak.
  Serial.print(" Luminosidade: ");
  Serial.print(lumi);
  Serial.print(" lux");
}

// Envia alerta para o IFTTT.
void alertaIFTTT() {
  HTTPClient client;

  // Constroi URL para enviar dados ao IFTTT.
  String url = String(IFTTT_URL) + IFTTT_API_KEY;

  // Especifica servidor, porta do processo e URL de envio de dados e inicia
  // um canal de comunicacao com o servidor.
  client.begin(IFTTT_HOST, HTTP_PORT, url);

  // Faz requisicao e guarda codigo de retorno.
  int codigoHTTP = client.GET();

  // Fecha o canal de comunicacao com o servidor.
  client.end();

  // Mostra codigo de retorno (se foi um sucesso ou fracasso, por exemplo).
  Serial.println(String("IFTTT retornou HTTP CODE ") + codigoHTTP);
}

// Envia dados para ThingSpeak.
void atualizaThingSpeak() {
  HTTPClient client;

  // Constroi URL para enviar dados ao ThingSpeak, com cada dado e chave da API.
  String url = String(TS_URL) +
    "?api_key=" + TS_API_KEY +
    "&field1=" + temp +
    "&field2=" + umidAr +
    "&field3=" + lumin;

  // Especifica servidor, porta do processo e URL de envio de dados e inicia
  // um canal de comunicacao com o servidor.
  client.begin(TS_HOST, HTTP_PORT, url);

  // Faz requisicao e guarda codigo de retorno.
  int codigoHTTP = client.GET();
  // Mostra codigo de retorno (se foi um sucesso ou fracasso, por exemplo).

  // Fecha o canal de comunicacao com o servidor.
  client.end();

  // Mostra codigo de retorno (se foi um sucesso ou fracasso, por exemplo).
  Serial.println(String("ThingSpeak retornou HTTP CODE ") + codigoHTPP);
}

void setup() {
  // Inicia sensor DHT11.
  dht.begin();

  // Configura WiFi.
  wifi.addAP(NOME_REDE, SENHA);

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
  leUmidadeDoSolo();
  Serial.print(" | ");

  // Le sensor de temperature e umidade do ar.
  leTemperaturaEUmidadeDoAr();
  Serial.print(" | ");

  // Le sensor de luminosidade.
  leLuminosidade();
  Serial.println(" | ");

  // Se a WiFi esta devidamente configurada e conectada.
  if (wifi.run() == WL_CONNECTED) {
    // Envia dados para o ThingSpeak e espera um periodo curto de tempo.
    atualizaThingSpeak();
    delay(500);

    // Se a umidade do solo estiver baixa (ou seja, o solo esta seco).
    if (umidSolo == 1) {
      // Envia alerta para IFTTT e espera um periodo curto de tempo
      alertaIFTTT();
      delay(300);
    }
  }

  // Espera 3seg para executar o proximo ciclo.
  delay(3000);
}

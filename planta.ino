#include "DHT.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#define DHTPIN D2 //pino ligado ao sensor DHT (umid e temp do ar)
#define DHTTYPE DHT11  //modelo do sensor, DHT11.
#define UMIDPIN D1 //pino sensor umidade de solo
#define LDRPIN A0 //pino sensor de luminosidade

//-----------------Config. ThingSpeak:
const int HTTP_PORT= 80;
const char* SSID = "<SSID>";
const char* PASSWORD = "<PASSWORD>";

const char* TS_HOST = "api.thingspeak.com";
const char* TS_URL = "/update";
const char* TS_API_KEY = "<TS_API_KEY>";
//------------------

//------------------Config. IFTTT:
const char* IFTTT_HOST = "maker.ifttt.com";
const char* IFTTT_URL = "/trigger/<EVENT>/with/key/";
const char* IFTTT_API_KEY = "<IFTTT_API_KEY>";
//------------------

//Variáveis globais, usadas para atualizar ThingSpeak
int umid_solo = 0;
int umid_ar = 0;
int temp = 0;
int lumin = 0;

ESP8266WiFiMulti wifi;
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  dht.begin(); //inicia sensor DHT11

  wifi.addAP(SSID, PASSWORD);

  pinMode(LED_BUILTIN, OUTPUT); //define pino do LED como saida
  pinMode(DHTPIN, INPUT);
  pinMode(UMIDPIN, INPUT);

  Serial.begin(115200);
  Serial.print("Setup iniciado");
  Serial.println("");
}

void loop() {

  umidade_solo(); //leitura de umidade de solo
    Serial.print(" | ");
 temp_umid(); //leitura sensor temp e umid AR
    Serial.print(" | ");
    luminosidade();
    Serial.print(" | ");
    Serial.println("");



 if (wifi.run() == WL_CONNECTED) {
    updateThingSpeak();
    delay(500);
    if(umid_solo ==  0){
    alertIFTTT();
    delay(300);
  }
  }
  delay(3000);
}

////--------------Sensor de Umid de Solo------------
void umidade_solo(){
  int umid = digitalRead(UMIDPIN); //lê o valor da entrada analógica
  umid_solo = umid; //variavel global para atualizar thingspeak
  delay(300);
  Serial.print("UmidSolo: ");
  Serial.print(umid); //1 = seco; 0 = umido.
}

////-------------Sensor temp e umid do ar---------
void temp_umid(){
  float h = dht.readHumidity();
  umid_ar = h; //variavel global para atualizar thingspeak
  float t = dht.readTemperature();
  temp = t; //variavel global para atualizar thingspeak
  delay(300);
  if (isnan(t) || isnan(h)){
    Serial.print("Failed to read from DHT");
  }
  else{
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.print(" %");
    Serial.print(" | ");
    Serial.print(" Temperatura: ");
    Serial.print(t);
    Serial.print(" *C");
  }
}
////------------Sensor LDR---------------------
void luminosidade(){
  float leitura_lumi = analogRead(LDRPIN); //lê o valor fornecido pelo LDR
  float lumi = map(leitura_lumi, 1023, 0, 0, 1000); //conversao para lux aproximada
  lumin = lumi; //variavel global para atualizar thingspeak
  Serial.print(" Luminosidade: ");
  Serial.print(lumi);
  Serial.print(" lux");
}



////--------------IFTTT------------------
void alertIFTTT() {
  HTTPClient client;
  String url = String(IFTTT_URL) + IFTTT_API_KEY;

  client.begin(IFTTT_HOST, HTTP_PORT, url);
  int httpCode = client.GET();

  Serial.println(String("IFTTT retornou HTTP CODE ") + httpCode);
}

////-----------ThingSpeak----------------------
void updateThingSpeak() {
  HTTPClient client;
  String url = String(TS_URL) +
    "?api_key=" + TS_API_KEY +
    "&field1=" + umid_ar +
    "&field2=" + umid_solo +
    "&field3=" + lumin +
    "&field4=" + temp;

  client.begin(TS_HOST, HTTP_PORT, url);
  int httpCode = client.GET();
  client.end();

  Serial.println(String("ThingSpeak retornou HTTP CODE ") + httpCode);
}

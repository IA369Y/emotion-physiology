#include <esp32-hal-i2c.h>
#include <Wire.h>
#include <SparkFunMLX90614.h>
#include "MAX30100.h"
#include <Arduino.h>
#include <math.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <iostream>
#include <string>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <math.h>

#define NTP_OFFSET  -14400 
#define NTP_INTERVAL 60 * 1000
#define NTP_ADDRESS  "a.st1.ntp.br"
#define OLED_RESET 4

MAX30100* pulseOxymeter;
Adafruit_SSD1306 display(OLED_RESET); 

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
WiFiClient espClient;
PubSubClient client(espClient);

IRTherm sensor;

const int GSR = A4;
const char* ssid = "SSID";//Nome da rede Wifi
const char* password = "PASSWORD_SSID";//Password da rede Wifi
const char* ssid2 = "SSID_2"; //Nome da rede wifi secundaria (opcional)
const char* password2 = "PASSWORD_SSID_2"; //Password da rede Wifi secundaria (opcional)
const char* mqtt_server = "IP_MQTT_SERVER";//Ip do broker MQTT
const char* clientname = "Esp32";

/*
IPAddress local_IP(0, 0, 0, 0);//Dados para manter ip fixo no wifi, caso utilize ip estatico remova o comentario e coloque os dados
IPAddress gateway(0, 0, 0, 0);
IPAddress subnet(0, 0, 0, 0);
IPAddress primaryDNS(0, 0, 0, 0); 
IPAddress secondaryDNS(0, 0, 0, 0); 
*/

String formattedTime, resultado;
long lastMsg = 0, test2 = 0;
int value = 0, sensorValue=0;
float Tempambiente, Tempreal, BPM, oxigenio, gsrindex, lTempambiente, lTempreal, lBPM, loxigenio, lgsrindex;
char tempString[8], temp2String[8], gsrString[8], bmpString[8], oxString[8], timeString[32], msg[50];

void readall(){
tempread();
gsrread();
}

void tempread(){
Tempambiente = sensor.ambient();
Tempreal = sensor.object();
}

void gsrread(){
    long sum=0;
    for(int i=0;i<100;i++)           
      {
      sensorValue=analogRead(GSR);
      sum += sensorValue;
      }
    gsrindex = (sum/100);
}

void serialresult(){

    Serial.print(timeClient.getEpochTime());
    Serial.print(";");
    Serial.print(tempString);
    Serial.print(";");
    Serial.print(temp2String);
    Serial.print(";");
    Serial.print(bmpString);
    Serial.print(";");
    Serial.print(oxString);
    Serial.print(";");
    Serial.println(gsrString);
}

void publicar(){

if (Tempambiente != lTempambiente) {  
    dtostrf(Tempambiente, 1, 2, tempString);
    client.publish("Emotiv/tempAmbiente", tempString);
    lTempambiente = Tempambiente;
}

if (Tempreal != lTempreal) { 
     dtostrf(Tempreal, 1, 2, temp2String);
    client.publish("Emotiv/tempCorporal", temp2String);
     lTempreal = Tempreal;
}

if (gsrindex != lgsrindex){
    dtostrf(gsrindex, 1, 2, gsrString);
    client.publish("Emotiv/gsr", gsrString);
    lgsrindex = gsrindex;
}

if (BPM != lBPM){
    dtostrf(BPM, 1, 2, bmpString);
    client.publish("Emotiv/bpm", bmpString);
    lBPM = BPM;
}

if (oxigenio != loxigenio){
    dtostrf(oxigenio, 1, 2, oxString);
    client.publish("Emotiv/ox", oxString);
    loxigenio = oxigenio;
}
    dtostrf(timeClient.getEpochTime(), 1, 2, timeString);
    client.publish("Emotiv/time", timeString);
}

void drawlcd(){ 
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Temp:");
  display.print(Tempreal);
  display.println("C");
  display.print("BPM:"); 
  display.print(BPM); 
  display.print(" "); 
  display.print("Ox:"); 
  display.print(oxigenio); 
  display.println("%");
  display.print("GSR:"); 
  display.println(gsrindex);
  display.print("Emotion:"); 
  display.print(resultado);  
  display.display();
 }

void setup() {
  Serial.begin(115200);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("  Emotiv");
  display.println("  Detect");
  display.display();
  delay (1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.println("Iniciando ");
  display.println("sensores...");
  display.display();
  sensor.begin();
  sensor.setUnit(TEMP_C);
  pulseOxymeter = new MAX30100();
  pulseOxymeter->printRegisters();
  delay (100);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println("Conectando");
  display.println(" Wifi...");
  display.display();
  
 if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
 Serial.println("STA Failed to configure");
 }
  setup_wifi();
  display.setCursor(0,0);
  display.clearDisplay();
  display.println("Conectando");
  display.println(" MQTT...");
  display.display();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println("Conectando");
  display.println(" NTP...");
  display.display();
  timeClient.begin();
  timeClient.update();
  formattedTime = timeClient.getFormattedTime();
  delay(1000);
  display.clearDisplay();
  Serial.print("A hora é: ");
  Serial.println(formattedTime);
  display.setCursor(0,0);
  display.println("Hora");
  display.println(formattedTime);
  display.display();
  delay (200);
  display.clearDisplay();
  display.setTextSize(1);
}
void checarwifi(){
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("O Wifi foi desconectado...refazendo configuração...");
    setup_wifi();
  }
}
void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  delay(5000);
  if (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Tentando Wifi 2...");
  Serial.print("Connecting to ");
  Serial.println(ssid2);
  WiFi.begin(ssid2, password2);
  delay(5000);
  if (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print("Tentando Wifi 1 novamente...");
  setup_wifi();
  }
}
  Serial.println("WiFi connected !");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
    for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (String(topic) == "Result/Result") {
    Serial.print("Recebido resultado: ");
    Serial.print(messageTemp);
    resultado = messageTemp;
      Serial.println("work");
     }
  }
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    Serial.print("MQTT Server is: ");
    Serial.println(mqtt_server);
    if (client.connect("clientname")) {
      Serial.println("connected");
      client.subscribe("/Result");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      checarwifi();
      client.setServer(mqtt_server, 1883);
      client.setCallback(callback);
      delay(1000);
    }
  }
}
void loop() {
    
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 400) {
    lastMsg = now;

    readall();
    publicar();
    drawlcd(); 
    serialresult();
  }

  long test = millis();
  if (test - test2 > 10) {
    pulseoxymeter_t result = pulseOxymeter->update();
    BPM = result.heartBPM;
    oxigenio = result.SaO2;
    test2 = test;
  }
}

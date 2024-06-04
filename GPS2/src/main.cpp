#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definir los pines para UART
#define MODEM_RX_PIN 20
#define MODEM_TX_PIN 21
#define MAX_TOKENS 20

// Definir los pines para la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Crear una instancia de la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Send data interval
int actualMillis = 0;
const int millisDiff = 25000;

// Your Domain name with URL path or IP address with path
const char* serverName = "https://electronicaupload.femat.dev/location/";

// Crear una instancia de HardwareSerial
HardwareSerial modem(1); // Utiliza UART1
String modemData;

//Serial checker
bool serialCheck = true;

void setup() {
  // Iniciar la comunicación serial con el PC
  unsigned long check = millis();
  unsigned long timeout = 2000;
  
  Serial.begin(115200);
  while (!Serial && serialCheck) { 
    if(millis() - check > timeout){
      serialCheck = false;
    }
   } // Esperar a que se inicie el puerto serial

  // Iniciar la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    if(serialCheck) Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Mostrar "Connecting" en la pantalla OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("Connecting");
  display.display();

  delay(10000);

  // Iniciar la comunicación serial con el módem
  modem.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  if(serialCheck) Serial.print("Comunicando con modem ");
  while(!modem){
    if(serialCheck) Serial.print(".");
  }

  while(modem.available()){
    modemData = modem.readStringUntil('\n');
    if(serialCheck) Serial.println(modemData);
  }

  // Iniciar GPS
  if(serialCheck) Serial.println("Iniciando comunicación con el módem...");
  bool atReady = false;
  
  // Establecer comunicación con HTTP
  if(serialCheck) Serial.println("Estableciendo fecha en el modem...");
  modem.println("AT+CTZU=1");
  while(!atReady){
    delay(500);
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      if(serialCheck) Serial.println(modemData);
    }
    if(modemData.startsWith("OK")){
        atReady = true;
    }
  }
  delay(1000);
  atReady = false;
  if(serialCheck) Serial.println("Iniciando servicio HTTP...");
  modem.println("AT+HTTPINIT");
  while(!atReady){
    delay(500);
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      if(serialCheck) Serial.println(modemData);
    }
    if(modemData.startsWith("OK")){
        atReady = true;
    }
    if(modemData.startsWith("ERROR")){
        atReady = true;
    }
  }

  atReady = false;
  if(serialCheck) Serial.print("Conectando con GPS");
  modem.println(" AT+CGNSSPWR=1");
  while(!atReady){
    delay(500);
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      if(serialCheck) Serial.println(modemData);
    }
    if(modemData.startsWith("OK")){
        atReady = true;
    }
    delay(5000);
  }

  // Limpiar la pantalla y prepararla para mostrar coordenadas
  display.clearDisplay();
  display.setTextSize(1);
  display.display();
}

void loop() {
  while(modem.available()){
    modemData = modem.readStringUntil('\n');
    if(modemData.startsWith("+HTTPACTION")){
      display.print(modemData);
      display.display();
    }
    if(serialCheck) Serial.println(modemData);
    delay(100);
  }
  
  if(millis() - actualMillis > millisDiff){ // Cada X segundos revisa el GPS y envía los datos
    modem.println("AT+CGPSINFO");
    delay(500);
    String sendTo;
    while(modem.available()){
      modemData = modem.readStringUntil('\n');
      if(serialCheck) Serial.println(modemData);
      if(modemData.startsWith("+CGPSINFO")){
        modemData=modemData.substring(11, modemData.length() - 1);
        String url = serverName;
        url.concat(modemData);
        url.concat("\"");
        sendTo = "AT+HTTPPARA=\"URL\",\"" + url;
        
        // Mostrar las coordenadas en la pantalla OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Coordinates:");
        display.setCursor(0, 10);
        display.print(modemData);
        display.display();
      }
    }
    delay(1000);
    modem.println(sendTo);
    while(modem.available()){
      String ok = modem.readStringUntil('\n');
      if(serialCheck) Serial.println(ok);
    }
    modem.println("AT+HTTPACTION=0");
    actualMillis = millis();
  }
  if(serialCheck){
    if (Serial.available()) {
      String pcData = Serial.readStringUntil('\n');
      Serial.print("Enviando mensaje: ");
      Serial.println(pcData);
      modem.println(pcData);
      delay(1000);
    }
  }
  
}

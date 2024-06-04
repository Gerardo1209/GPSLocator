#include <Arduino.h>

// Definir los pines para UART
#define MODEM_RX_PIN 20
#define MODEM_TX_PIN 21
#define MAX_TOKENS 20

//Send data interval
int actualMillis = 0;
const int millisDiff = 25000;

//Your Domain name with URL path or IP address with path
const char* serverName = "https://electronicaupload.femat.dev/location/";

// Crear una instancia de HardwareSerial
HardwareSerial modem(1); // Utiliza UART1
String modemData;

void setup() {
  // Iniciar la comunicación serial con el PC
  Serial.begin(115200);
  while (!Serial) { ; } // Esperar a que se inicie el puerto serial

  delay(10000);
  //Inicar WiFi
  /*;
  Serial.print("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(WiFi.status());
    //Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());*/
  /*WiFi.mode(WIFI_STA);
  WiFi.disconnect();*/


  // Iniciar la comunicación serial con el módem
  modem.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  Serial.print("Comunicando con modem ");
  while(!modem){
    Serial.print(".");
  }

  
  while(modem.available()){
    modemData = modem.readStringUntil('\n');
    Serial.println(modemData);
  }

  //Iniciar GPS
  Serial.println("Iniciando comunicación con el módem...");
  bool atReady = false;
  
  //Establecer comunicación con HTTP
  Serial.println("Estableciendo fecha en el modem...");
  modem.println("AT+CTZU=1");
  while(!atReady){
    delay(500);
    
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      Serial.println(modemData);
    }
    if(modemData.startsWith("OK")){
        atReady = true;
    }
    
  }
  delay(1000);
  atReady = false;
  Serial.println("Iniciando servicio HTTP...");
  modem.println("AT+HTTPINIT");
  while(!atReady){
    delay(500);
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      Serial.println(modemData);
    }
    if(modemData.startsWith("OK")){
        atReady = true;
    }
    if(modemData.startsWith("ERROR")){
        atReady = true;
    }
  }

  atReady = false;
  Serial.print("Conectando con GPS");
  modem.println(" AT+CGNSSPWR=1");
  while(!atReady){
    delay(500);
    //Serial.print(".");
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      Serial.println(modemData);
    }
    if(modemData.startsWith("OK")){
        atReady = true;
    }
    delay(5000);
    /*if(modemData.compareTo("+CGNSSPWR: READY!") == 13){
      Serial.println("Configurando comunicación del GPS");
      delay(100);
      modem.println("AT+CGNSSPORTSWITCH=1,1");
      delay(400);
      atReady = true;
    }*/
  }
}

void loop() {
  /*if (WiFi.status() == WL_CONNECTED) { // Verifica si está conectado a WiFi
    HTTPClient http;
    WiFiClient client;
    http.begin(client, serverName); // URL del servidor
    int httpCode = http.GET(); // Realiza la petición

    if (httpCode > 0) { // Si la petición fue exitosa
      String payload = http.getString(); // Obtiene el cuerpo de la respuesta
      Serial.println(payload); // Imprime el contenido de la respuesta
    } else {
      Serial.println("Error en la petición HTTP");
    }

    http.end(); // Libera los recursos
  }
  delay(1000);*/
    // Leer datos del módem y enviarlos al puerto serial del PC

  while(modem.available()){
    modemData = modem.readStringUntil('\n');
    Serial.println(modemData);
    delay(100);
  }
  if(millis() - actualMillis > millisDiff){ //Cada X segundos revisa el GPS y envia los datos
    modem.println("AT+CGPSINFO");
    delay(500);
    String sendTo;
    while(modem.available()){
      modemData = modem.readStringUntil('\n');
      Serial.println(modemData);
      if(modemData.startsWith("+CGPSINFO")){
        modemData=modemData.substring(11, modemData.length() - 1);
        String url = serverName;
        url.concat(modemData);
        url.concat("\"");
        sendTo = "AT+HTTPPARA=\"URL\",\"" + url;
      }
    }
    delay(1000);
    modem.println(sendTo);
    while(modem.available()){
      String ok = modem.readStringUntil('\n');
      Serial.println(ok);
    }
    modem.println("AT+HTTPACTION=0");
    actualMillis = millis();
  }
  // Leer datos del puerto serial del PC y enviarlos al módem
  if (Serial.available()) {
      String pcData = Serial.readStringUntil('\n');
      Serial.print("Envaindo mensaje: ");
      Serial.println(pcData);
      modem.println(pcData);
      delay(1000);
  }
}

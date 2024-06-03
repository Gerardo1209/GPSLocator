#include <WiFi.h>
#include <HTTPClient.h>

// Definir los pines para UART
#define MODEM_RX_PIN 20
#define MODEM_TX_PIN 21
#define MAX_TOKENS 20

//Funciones
void httpGETRequest(String serverName);

//Wifi data
const char* ssid = "geras";
const char* password = "holis1234;";
WiFiClient client;

//Send data interval
int actualMillis = 0;
const int millisDiff = 10000;

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.215.66:3000/location/";

// Crear una instancia de HardwareSerial
HardwareSerial modem(1); // Utiliza UART1

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
  delay(100);


  // Iniciar la comunicación serial con el módem
  modem.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  Serial.print("Comunicando con modem ");
  while(!modem){
    Serial.print(".");
  }

  //Iniciar GPS
  Serial.println("Iniciando comunicación con el módem...");
  bool atReady = false;
  
  //Establecer comunicación con paginas web
  Serial.println("Estableciendo hora en el modem...");
  modem.println("AT+CTZU=1");
  while(!atReady){
    delay(500);
    String modemData;
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      Serial.println(modemData);
    }
    if(modemData.startsWith("OK")){
        atReady = true;
    }
  }

  atReady = false;
  Serial.print("Conectando con GPS");
  modem.println("AT+CGNSSPWR=1");
  while(!atReady){
    delay(500);
    //Serial.print(".");
    String modemData;
    if (modem.available()){
      modemData = modem.readStringUntil('\n');
      Serial.println(modemData);
    }
    if(modemData.startsWith("$G")){
        atReady = true;
    }
    if(modemData.compareTo("+CGNSSPWR: READY!") == 13){
      Serial.println("Configurando comunicación");
      delay(100);
      modem.println("AT+CGNSSPORTSWITCH=1,1");
      delay(400);
      modem.println("AT+CGNSSTST=1");
      delay(400);
      atReady = true;
    }
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
    if (modem.available()) {
        String modemInfo = modem.readStringUntil('\n');
        Serial.println(modemInfo);
        if(modemInfo.startsWith("$GNRMC")){
            
            
            //httpGETRequest(serverName);
            String tokens[MAX_TOKENS];
            int tokenCount = 0;
            int startIndex = 0;
            int endIndex = modemInfo.indexOf(',');

            while (endIndex != -1) {
                // Extraer el token
                tokens[tokenCount] = modemInfo.substring(startIndex, endIndex);
                tokenCount++;
                startIndex = endIndex + 1;
                endIndex = modemInfo.indexOf(',', startIndex);
            }
            // Agregar el último token después de la última coma
            if (startIndex < modemInfo.length()) {
                tokens[tokenCount] = modemInfo.substring(startIndex);
                tokenCount++;
            }
            //Envia al servidor los datos según los millis determinados
            if(millis() - actualMillis > millisDiff){
              actualMillis = millis();
              String url = serverName;
              url.concat(tokens[1] + "," + tokens[2] + "," + tokens[3] + "," + tokens[4] + "," + tokens[5] + "," + tokens[6]);
              if (WiFi.status() == WL_CONNECTED) { // Verifica si está conectado a WiFi
                HTTPClient http;
                http.begin(client, url); // URL del servidor
                int httpCode = http.GET(); // Realiza la petición
                if (httpCode > 0) { // Si la petición fue exitosa
                  String payload = http.getString(); // Obtiene el cuerpo de la respuesta
                  Serial.println(payload); // Imprime el contenido de la respuesta
                } else {
                  Serial.println("Error en la petición HTTP");
                }
                http.end(); // Libera los recursos
              }
            }
            
        }
    }

    // Leer datos del puerto serial del PC y enviarlos al módem
    if (Serial.available()) {
        String pcData = Serial.readStringUntil('\n');
        Serial.print("Envaindo mensaje: ");
        Serial.println(pcData);
        modem.println(pcData);
    }
}

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESPmDNS.h>

const char* ssid = "yourssid";
const char* password = "yourpass";
const IPAddress staticIP(192, 168, 1, 69);
const IPAddress gateway(192, 168, 10, 1);
const IPAddress subnet(255, 255, 255, 0);
#define DHTPIN 27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


AsyncWebServer server(80);

void handleOn(AsyncWebServerRequest *request){
  digitalWrite(25, LOW);
  request->send(200, "text/html", "<html><script>window.location.href = '/';</script></html>");
}

void handleOff(AsyncWebServerRequest *request){
  digitalWrite(25, HIGH);
  request->send(200, "text/html", "<html><script>window.location.href = '/';</script></html>");
}

void handleOn2(AsyncWebServerRequest *request){
  digitalWrite(26, LOW);
  request->send(200, "text/html", "<html><script>window.location.href = '/';</script></html>");
}

void handleOff2(AsyncWebServerRequest *request){
  digitalWrite(26, HIGH);
  request->send(200, "text/html", "<html><script>window.location.href = '/';</script></html>");
}

void setup() {
  dht.begin();
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  if (!MDNS.begin("webserver")){
    Serial.println("Error configurando mDNS");
    while (1){
      delay(1000);
    }
  }  
  Serial.println("mDNS configurado");

  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/on2", handleOn2);
  server.on("/off2", handleOff2);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    request->send(400, "text/plain", "Error al leer los datos del sensor");
    return;
  }
  String s = "<html>"
             "<head>"
               "<style>"
                 "body { font-family: Arial, sans-serif; text-align: center; background-color: #333; color: white; }"
                 "h1 { font-size: 36px; color: #4E4E4E; margin-top: 50px; text-shadow: #fff 1px 0 10px; }"
                 "button { padding: 10px 20px; font-size: 18px; margin-top: 20px; background-color: #ccc; border-radius: 20px; }"
                 "p { font-size: 24px; color: #fff; margin-top: 50px; }"
               "</style>"
             "</head>"
             "<body>"
               "<h1>Cuarto Webserver</h1>"
               "<p>Temperatura: " + String(t) + "</p>"
               "<p>Humedad: " + String(h) + "%</p>"
               "<br>"
               "<button onclick=\"location.href='/on'\">On</button>"
               "<button onclick=\"location.href='/off'\">Off</button>"
               "<br>"
               "<button onclick=\"location.href='/on2'\">On</button>"
               "<button onclick=\"location.href='/off2'\">Off</button>"
             "</body>"
           "</html>";
  request->send(200, "text/html", s);
});

  server.begin();

  MDNS.addService("http", "tcp", 80);
}

void loop() {
  delay(5000);
}


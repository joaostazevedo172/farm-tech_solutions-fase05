/*
  🚜 FarmTech Solutions - Projeto "Ir Além"
  Sistema: Monitoramento Híbrido (Web + Serial Dashboard)
  
  HARDWARE:
  1. DHT22 (Temp/Umidade) -> Pino 15
  2. LDR (Luz) -> Pino 34
*/

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

#define DHTPIN 15
#define DHTTYPE DHT22
#define LDRPIN 34

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Timer para atualizar o Serial sem travar o WiFi
unsigned long ultimoTempo = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LDRPIN, INPUT);

  Serial.println("--- INICIANDO FARMTECH SYSTEM ---");
  Serial.print("Conectando Wi-Fi");
  WiFi.begin(ssid, password);
  
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }
  
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\n✅ Wi-Fi ON!");
    Serial.print("IP: http://");
    Serial.println(WiFi.localIP());
    
    // --- AQUI COMEÇA A MUDANÇA (DASHBOARD VISUAL) ---
    server.on("/", []() {
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      int luz = analogRead(LDRPIN);
      
      // Lógica simples para status (só visual)
      String corCard = (t > 30) ? "#ffcccc" : "#ccffcc"; // Vermelho se quente, Verde se normal
      
      String html = "<!DOCTYPE html><html>";
      html += "<head>";
      html += "<meta charset='UTF-8'>";
      html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
      html += "<meta http-equiv='refresh' content='3'>"; // <--- ATUALIZA SOZINHO A CADA 3 SEG
      html += "<style>";
      html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }";
      html += ".card { background: white; padding: 20px; margin: 10px auto; width: 80%; max-width: 400px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
      html += "h1 { color: #2c3e50; }";
      html += ".value { font-size: 24px; font-weight: bold; color: #27ae60; }";
      html += "</style>";
      html += "</head><body>";
      
      html += "<h1>🚜 FarmTech Monitor</h1>";
      
      html += "<div class='card' style='background-color: " + corCard + "'>";
      html += "<h3>🌡️ Temperatura</h3>";
      html += "<p class='value'>" + String(t, 1) + " °C</p>";
      html += "</div>";
      
      html += "<div class='card'>";
      html += "<h3>💧 Umidade</h3>";
      html += "<p class='value'>" + String(h, 0) + " %</p>";
      html += "</div>";
      
      html += "<div class='card'>";
      html += "<h3>☀️ Luminosidade (LDR)</h3>";
      html += "<p class='value'>" + String(luz) + "</p>";
      html += "</div>";
      
      html += "<p><small>Atualizando automaticamente...</small></p>";
      html += "</body></html>";
      
      server.send(200, "text/html", html);
    });
    // --- FIM DA MUDANÇA ---

    server.begin();
  } else {
    Serial.println("\n❌ Erro no Wi-Fi (Simulador instavel).");
    Serial.println("Iniciando Modo Offline...");
  }
}

void loop() {
  server.handleClient(); // Tenta manter o site vivo
  
  // Atualiza o Painel Serial a cada 2 segundos
  if (millis() - ultimoTempo > 2000) {
    ultimoTempo = millis();
    
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int luz = analogRead(LDRPIN);
    String statusLuz = (luz < 1000) ? "ENSOLARADO ☀️" : (luz < 2500) ? "NUBLADO ☁️" : "ESCURO 🌙";

    // DESENHA O PAINEL NO TERMINAL
    Serial.println("\n=================================");
    Serial.println("🚜  FARMTECH SOLUTIONS - MONITOR");
    Serial.println("=================================");
    Serial.print("🌡️  TEMPERATURA:   "); Serial.print(t); Serial.println(" °C");
    Serial.print("💧  UMIDADE:       "); Serial.print(h); Serial.println(" %");
    Serial.print("💡  LUMINOSIDADE:  "); Serial.println(statusLuz);
    Serial.println("---------------------------------");
    
    // Aviso visual se a temperatura estiver critica
    if (t > 30) Serial.println("⚠️  ALERTA: TEMPERATURA ALTA!");
    if (h < 40) Serial.println("⚠️  ALERTA: BAIXA UMIDADE!");
    Serial.println("=================================\n");
  }
  delay(10); // Pequeno delay para estabilidade do Wokwi
}
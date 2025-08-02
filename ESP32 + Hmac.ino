#include <WiFi.h>
#include <HTTPClient.h>
#include <SHA256.h>  // Include the SHA256 library
#include <DHT.h>     // Include the DHT library

const char* ssid = "HONOR 90";
const char* password = "R11223344";
const char* server = "http://192.168.252.36:5000/data";  // Flask server URL

#define DHTPIN 4       // DHT11 data pin connected to GPIO 4
#define DHTTYPE DHT11  // DHT sensor type
DHT dht(DHTPIN, DHTTYPE);

int sensorPin = 34;  // MQ2 analog pin connected to GPIO 34 on ESP32
String secret = "my_shared_secret_key";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  dht.begin();  // Initialize DHT sensor
}

void loop() {
  // Read MQ2 sensor value
  int Gas_value = analogRead(sensorPin);

  // Read DHT11 sensor values
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if DHT11 readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print sensor values to the Serial Monitor
  Serial.print("gas Value: ");
  Serial.println(Gas_value);
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Combine sensor data into a single string for hashing
  String data = String(Gas_value) + "," + String(humidity, 2) + "," + String(temperature, 2);

  // Create SHA-256 hash
  SHA256 sha256;
  String dataWithSecret = data + secret;  // Append secret to data
  sha256.update((const uint8_t*)dataWithSecret.c_str(), dataWithSecret.length());
  
  uint8_t hashBytes[32];  // SHA-256 produces a 32-byte hash
  sha256.finalize(hashBytes, sizeof(hashBytes));

  // Convert hash bytes to a hex string
  String hashString = "";
  for (int i = 0; i < 32; i++) {
    if (hashBytes[i] < 0x10) hashString += "0";  // Add leading zero for formatting
    hashString += String(hashBytes[i], HEX);
  }

  // Print the hash to the Serial Monitor
  Serial.print("Hash: ");
  Serial.println(hashString);

  // Create JSON payload with sensor values and hash
  String jsonData = "{\"gas_value\": " + String(Gas_value) + 
                    ", \"humidity\": " + String(humidity) + 
                    ", \"temperature\": " + String(temperature) + 
                    ", \"hash\": \"" + hashString + "\"}";

  // Print the JSON payload to the Serial Monitor
  Serial.print("JSON Payload: ");
  Serial.println(jsonData);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      Serial.print("Data sent. Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Server Response: ");
      Serial.println(http.getString());  // Display server response
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
  }

  delay(20000);  // Send data every 15 seconds
}
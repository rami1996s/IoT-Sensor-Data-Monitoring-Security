#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>  // Include the DHT library

// WiFi credentials
const char* ssid = "HONOR 90";
const char* password = "R11223344";

// Flask server URL to send data
const char* server = "http://192.168.252.36:5000/data";  // Replace with Flask server IP

// DHT11 sensor setup
#define DHTPIN 4        // GPIO pin where DHT11 is connected
#define DHTTYPE DHT11   // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// MQ2 sensor pin
#define MQ2_PIN 34      // Analog pin where MQ2 is connected

// WiFi setup
WiFiClient espClient;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Read MQ2 sensor value
  int Gas_value = analogRead(MQ2_PIN);
  Serial.print("gas Value: ");
  Serial.println(Gas_value);

  // Read DHT11 sensor values
  float humidity = dht.readHumidity();      // Read humidity
  float temperature = dht.readTemperature();  // Read temperature in Celsius

  // Check if DHT11 readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print DHT11 values to Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Send data to Flask server using HTTP POST
  sendDataToFlaskServer(Gas_value, humidity, temperature);

  // Wait for 15 seconds before sending the next batch of data
  delay(15000);
}

void sendDataToFlaskServer(int Gas_value, float humidity, float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server);
    http.addHeader("Content-Type", "application/json");

    // Prepare JSON payload
    String jsonData = "{\"gas_value\": " + String(Gas_value) + 
                      ", \"humidity\": " + String(humidity) + 
                      ", \"temperature\": " + String(temperature) + "}";

    // Send POST request to Flask server
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      Serial.print("Data sent to Flask server. Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
  }
}

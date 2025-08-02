from flask import Flask, request, jsonify
import certifi
import ssl
import mysql.connector
import paho.mqtt.client as mqtt
import json

app = Flask(__name__)

# MySQL Database Configuration
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="Hsko&7890",
    database="sensor_data"
)
cursor = db.cursor()

# HiveMQ Cloud MQTT Configuration
MQTT_BROKER = "32a402294f504c7499c00d5ceac7526e.s1.eu.hivemq.cloud"
MQTT_PORT = 8883
MQTT_TOPIC = "esp32/sensor"
MQTT_USERNAME = "cloud"
MQTT_PASSWORD = "Sa12345678"

# Initialize MQTT Client
mqtt_client = mqtt.Client()
mqtt_client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
mqtt_client.tls_set(certifi.where(), tls_version=ssl.PROTOCOL_TLS)
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

@app.route('/data', methods=['POST'])
def receive_data():
    try:
        data = request.json
        gas_value = data.get('gas_value')      # Renamed from sensor_value
        humidity = data.get('humidity')
        temperature = data.get('temperature')

        if gas_value is None or humidity is None or temperature is None:
            return jsonify({"error": "Invalid data format"}), 400

        # Insert data into MySQL
        sql = "INSERT INTO readings (gas_value, humidity, temperature) VALUES (%s, %s, %s)"
        values = (gas_value, humidity, temperature)
        cursor.execute(sql, values)
        db.commit()

        # Publish to MQTT
        payload = json.dumps({
            "gas_value": gas_value,
            "humidity": humidity,
            "temperature": temperature
        })
        mqtt_client.publish(MQTT_TOPIC, payload)

        return jsonify({"message": "Data received and published to MQTT"}), 201

    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/data', methods=['GET'])
def get_data():
    cursor.execute("SELECT * FROM readings ORDER BY timestamp DESC LIMIT 10")
    rows = cursor.fetchall()
    data = [{
        "id": row[0],
        "gas_value": row[1],        # Updated key
        "humidity": row[2],
        "temperature": row[3],
        "timestamp": row[4].strftime("%Y-%m-%d %H:%M:%S")
    } for row in rows]

    return jsonify(data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
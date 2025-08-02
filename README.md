# IoT Sensor Data Monitoring System (Security-Focused)

This project demonstrates a secure IoT system for real-time sensor data monitoring using ESP32 microcontrollers and a Flask server. It implements cryptographic and network defenses to ensure data integrity, authenticity, and availability in the face of common IoT attacks.

---

##  Overview

- Collects sensor data using:
  - **DHT11** for temperature and humidity
  - **MQ2** for gas detection
- Transmits data to a Flask server over HTTP
- Secures data with **HMAC-SHA256** to ensure integrity and authenticity
- Mitigates DoS attacks using **firewall rules**
- Stores data in **MySQL**
- Publishes data to **Node-RED dashboard** via **MQTT (HiveMQ Cloud)**

---

##  Key Security Features

| Threat                 | Mitigation                         |
|------------------------|-------------------------------------|
| Data Tampering         | HMAC-SHA256 (using a shared secret) |
| Message Forgery        | Secret-based hash validation        |
| Denial of Service (DoS)| iptables/UFW firewall rules         |
| Packet Sniffing        | Hash prevents modification validity |

---

##  System Components

###  Hardware

- **ESP32** – Sensor data collection and signing
- **MQ2 Gas Sensor**
- **DHT11 Sensor**

### ☁️ Software & Services

- **Flask** – HTTP server with hash verification logic
- **MySQL** – Data storage
- **Node-RED** – Real-time dashboard via MQTT
- **HiveMQ Cloud** – MQTT broker
- **Wireshark** – Attack simulation
- **iptables/UFW** – Network defense

---

##  Data Flow (Normal Scenario)

1. **ESP32** collects temperature, humidity, and gas levels.
2. It generates a **HMAC-SHA256 hash** using a secret key.
3. Sends data + hash as JSON via HTTP to Flask server.
4. Flask verifies the hash:
   - If valid → stores in MySQL and publishes to MQTT broker.
   - If invalid → rejects request.
5. Node-RED subscribes to MQTT topic and displays data live.

---

##  Attack Simulations

###  DoS Attack

- Simulated by flooding Flask server
- Mitigation:
  - **Rate limiting**
  - **Firewall rules** to block IPs

###  Data Tampering

- Simulated with tools like Wireshark or a fake client
- Mitigation:
  - Flask rejects altered data with invalid hash
  - HMAC ensures integrity and authenticity

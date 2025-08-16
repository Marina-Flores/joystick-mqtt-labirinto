# ESP-MQTT Sample Application — Academic Project

This project was developed as part of an academic activity in the Embedded Systems course of the Systems Analysis and Development program at IFPE Campus Garanhuns.

The objective was to use the ESP-MQTT library with the MQTT v5 protocol to implement communication between an **ESP32** and a **local MQTT broker (Mosquitto)**, allowing the control of an onboard LED on the ESP32 via messages published using **MQTTX**.

## Project Overview

This example connects to the broker configured via `idf.py menuconfig`, subscribes to an MQTT topic, and responds to received commands with actions on the ESP32, such as turning the onboard LED (GPIO2) on or off.

The LED is controlled through messages received on the topic:

```
/ifpe/ads/embarcados/esp32/led
```

Expected messages:

* "1": turns the LED on.
* "0": turns the LED off.

---

## Technologies Used

* **ESP32 (WROOM-32D)**
* **ESP-IDF 5.4.1**
* **Mosquitto** as local MQTT broker
* **MQTTX** for message publishing
* **MQTT v5** enabled via `menuconfig`

---

## Installing Mosquitto and MQTTX

### Mosquitto (Local Broker)

#### On Windows:

* Download the installer from: [https://mosquitto.org/download/](https://mosquitto.org/download/)
* Follow the installation instructions.
* Start the broker from the Command Prompt:

```bash
mosquitto -v
```

### MQTTX (MQTT Client)

* Download and install from: [https://mqttx.app/](https://mqttx.app/)
* Available for Windows, macOS, and Linux.

---

## Project Configuration

1. Configure the Wi-Fi connection using the menu:

   ```bash
   idf.py menuconfig
   ```

   Navigate to **Example Connection Configuration** and enter your Wi-Fi SSID and password (2.4 GHz).

2. Then configure the **MQTT broker**:

   * Address: `mqtt://localhost` or the IP of the machine running Mosquitto.

3. Make sure the MQTT v5 protocol is enabled:

   * Go to `ESP-MQTT Configurations` → enable `CONFIG_MQTT_PROTOCOL_5`

---

## Build and Flash

To build and flash the code to the ESP32:

```bash
idf.py build
idf.py -p /COMx flash monitor
```

(Replace `/COMx` with the correct port.)

To exit the monitor:

```
Ctrl + ]
```

---

## Testing with MQTTX

1. Open MQTTX and connect to the Mosquitto broker (localhost or IP).
2. Publish messages to the topic:

```
Topic: /ifpe/ads/embarcados/esp32/led
Payload: 1
```

or

```
Payload: 0
```

You should see the response in the serial terminal and the LED change its state (if available).

---

## Notes

* The main focus of the activity was to demonstrate MQTT communication between the ESP32 and the broker.

---

## Credits

Project based on the official Espressif example and adapted for educational purposes by \[Marina Flores] — IFPE/ADS 2025.

---

# IIoT Tank Monitoring System

## 📌 Project Overview

The **IIoT Tank Monitoring System** is an ESP32-based smart monitoring system designed to measure and monitor the level of liquid inside a tank.

The system uses an **HC-SR04 ultrasonic sensor** to measure the distance between the sensor and the liquid surface and calculates the corresponding tank level percentage. An **IR sensor** is also used for object detection. The current tank information is displayed locally on a **16×2 LCD** and transmitted to the **ThingSpeak cloud platform** through Wi-Fi for remote monitoring.

The project demonstrates the integration of **sensors, microcontrollers, embedded programming, LCD interfacing, Wi-Fi communication and cloud-based IoT monitoring**.

---

## 🎯 Objectives

* Monitor tank liquid level in real time.
* Measure the distance between the ultrasonic sensor and liquid surface.
* Calculate the approximate tank level as a percentage.
* Detect objects using an IR sensor.
* Display sensor information on a 16×2 LCD.
* Send sensor data to ThingSpeak through Wi-Fi.
* Provide both local and remote monitoring.

---

## 🛠️ Technologies Used

### Hardware

* ESP32
* HC-SR04 Ultrasonic Sensor
* IR Sensor
* 16×2 Parallel LCD
* Connecting wires
* Power supply

### Software

* Arduino IDE
* Embedded C/C++
* Wi-Fi
* ThingSpeak
* Arduino Libraries:

  * WiFi
  * ThingSpeak
  * Wire
  * LiquidCrystal

---

## 🔌 Pin Configuration

| Component    | ESP32 Pin |
| ------------ | --------- |
| HC-SR04 TRIG | GPIO 5    |
| HC-SR04 ECHO | GPIO 18   |
| IR Sensor    | GPIO 36   |
| LCD RS       | GPIO 23   |
| LCD Enable   | GPIO 22   |
| LCD D4       | GPIO 19   |
| LCD D5       | GPIO 21   |
| LCD D6       | GPIO 32   |
| LCD D7       | GPIO 33   |

---

## ⚙️ Working Principle

### 1. Ultrasonic Distance Measurement

The HC-SR04 sensor sends an ultrasonic pulse toward the liquid surface.

The ESP32 measures the time taken for the echo to return.

The distance is calculated using:

`Distance = Time × Speed of Sound / 2`

The measured distance is then used to calculate the tank level.

---

### 2. Tank Level Calculation

The system assumes a tank height of **100 cm**.

The approximate liquid level is calculated using:

`Level (%) = ((Tank Height - Distance) / Tank Height) × 100`

The calculated value is restricted between **0% and 100%**.

---

### 3. IR Sensor

The IR sensor is used for object detection.

The system interprets:

* `LOW` → Object detected
* `HIGH` → No object detected

The status is displayed on the LCD and transmitted to ThingSpeak.

---

### 4. LCD Display

The 16×2 LCD provides local real-time information.

Example:

`D:35cm L:65%`

`IR: No Object`

Where:

* `D` = measured distance
* `L` = calculated liquid level

---

### 5. Cloud Monitoring

The ESP32 connects to Wi-Fi and communicates with ThingSpeak.

The following values are uploaded:

* Field 1 → Ultrasonic distance
* Field 2 → Tank level percentage
* Field 3 → IR sensor status

The system updates ThingSpeak periodically while continuing to monitor the sensors locally.

---

# 🔄 System Flow

```text
          ┌─────────────────┐
          │    HC-SR04      │
          │ Ultrasonic      │
          │    Sensor       │
          └────────┬────────┘
                   │
                   ▼
          ┌─────────────────┐
          │      ESP32      │
          │ Sensor Reading  │
          │ & Processing    │
          └───────┬─────────┘
                  │
        ┌─────────┼──────────┐
        │         │          │
        ▼         ▼          ▼
   ┌────────┐ ┌────────┐ ┌─────────────┐
   │  LCD   │ │IR Sensor│ │   Wi-Fi     │
   │Display │ │Detection│ │     ↓       │
   └────────┘ └────────┘ │ ThingSpeak  │
                          └─────────────┘
```

---

# 🧪 Detailed Operating Scenarios

## Scenario 1 — Normal Tank Monitoring

### Condition

The tank contains a normal amount of liquid and the ultrasonic sensor receives a valid echo.

### System Response

1. ESP32 reads the ultrasonic sensor.
2. Distance is calculated.
3. Tank level percentage is calculated.
4. LCD displays distance and level.
5. IR sensor status is checked.
6. Data is periodically sent to ThingSpeak.

### Example

```text
Distance: 30 cm
Tank Level: 70%
IR: No Object
```

This represents the normal operating condition of the system.

---

## Scenario 2 — High Tank Level

### Condition

The liquid level rises close to the ultrasonic sensor.

### System Response

The measured distance becomes smaller.

Since the distance from the sensor to the liquid decreases, the calculated tank level increases.

Example:

```text
Distance: 10 cm
Tank Level: 90%
```

The updated level is displayed on the LCD and transmitted to ThingSpeak.

---

## Scenario 3 — Low Tank Level

### Condition

The tank contains a small amount of liquid.

### System Response

The distance between the ultrasonic sensor and liquid surface increases.

The calculated tank level therefore decreases.

Example:

```text
Distance: 85 cm
Tank Level: 15%
```

The user can monitor the decreasing level locally and through the ThingSpeak dashboard.

---

## Scenario 4 — Empty or Nearly Empty Tank

### Condition

The liquid level approaches the bottom of the tank.

### System Response

The ultrasonic sensor measures a distance close to the configured tank height.

The calculated percentage approaches 0%.

Example:

```text
Distance: 98 cm
Tank Level: 2%
```

The system continues monitoring and reporting the sensor values.

---

## Scenario 5 — Object Detected by IR Sensor

### Condition

An object is detected by the IR sensor.

### System Response

The IR sensor produces a LOW signal.

The ESP32 converts this into an object-detected status.

The LCD displays:

```text
IR: Object
```

ThingSpeak also receives the corresponding IR status.

---

## Scenario 6 — No Object Detected

### Condition

There is no object in front of the IR sensor.

### System Response

The IR sensor produces a HIGH signal.

The LCD displays:

```text
IR: No Object
```

The cloud status is updated accordingly.

---

## Scenario 7 — Ultrasonic Sensor Error

### Condition

The HC-SR04 does not receive an echo within the specified timeout.

### System Response

The system treats the measurement as invalid.

The LCD displays:

```text
Ultra ERROR
```

The system also sends an invalid measurement indicator to ThingSpeak rather than treating the missing echo as a valid distance.

---

## Scenario 8 — Wi-Fi Disconnection

### Condition

The ESP32 loses its Wi-Fi connection.

### System Response

Before sending cloud data, the system checks the Wi-Fi connection.

If disconnected, it attempts to reconnect.

Local sensor monitoring and LCD operation can continue independently of the cloud update.

---

## Scenario 9 — ThingSpeak Update Failure

### Condition

The ESP32 is unable to successfully write data to ThingSpeak.

### System Response

The response code from ThingSpeak is checked.

A successful update produces a success message in the Serial Monitor.

If the update fails, the system prints:

```text
ThingSpeak update failed.
```

This allows the developer to identify cloud communication problems during debugging.

---

# ⏱️ Timing

The system uses different intervals for local monitoring and cloud communication.

### Sensor Reading

Sensor values are processed approximately every:

```text
500 ms
```

### ThingSpeak Update

Cloud data is sent approximately every:

```text
15 seconds
```

This allows frequent local monitoring while avoiding unnecessarily frequent cloud updates.

---

# ☁️ ThingSpeak Data

The system sends three main parameters:

| ThingSpeak Field | Data             |
| ---------------- | ---------------- |
| Field 1          | Distance in cm   |
| Field 2          | Tank Level (%)   |
| Field 3          | IR Sensor Status |

The ThingSpeak status message also indicates whether an object was detected.

---

# 🖥️ Serial Monitor

The Serial Monitor can be used for debugging and observing system operation.

It provides:

* Distance
* Tank level
* IR sensor status
* Wi-Fi RSSI
* ThingSpeak response code
* Cloud update status

---

# 🔐 Security Note

**Do not upload real Wi-Fi credentials or ThingSpeak API keys to a public GitHub repository.**

Before uploading the project, replace private credentials with placeholders such as:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* THINGSPEAK_WRITE_API_KEY = "YOUR_THINGSPEAK_API_KEY";
```

---

# 🚀 Future Improvements

Possible future enhancements include:

* Automatic low-level and high-level alerts
* Mobile notifications
* Email/SMS alerts
* Relay-controlled water pump
* Automatic pump ON/OFF control
* Historical tank-level analytics
* Web dashboard
* Multiple tank monitoring
* OLED/TFT display
* Battery-powered operation
* Improved sensor error handling
* Data logging and analysis

---

# 📚 Learning Outcomes

This project provides practical experience in:

* Embedded C/C++ programming
* ESP32 microcontroller programming
* Sensor interfacing
* Ultrasonic distance measurement
* IR sensor interfacing
* LCD interfacing
* Digital I/O
* Wi-Fi communication
* IoT architecture
* Cloud data visualization
* Debugging using Serial Monitor

---

# 👩‍💻 Author

**Tanvi Dahale**

Electronics & Telecommunication Engineering
Cummins College of Engineering for Women, Pune

# 🔥 Forest Fire Detection System (IoT + Full Stack)

An IoT-based real-time forest fire detection system using ESP32, multiple sensors, Firebase, and a web dashboard. The system continuously monitors environmental conditions and sends alerts when fire risk is detected.

---

## 🚀 Features

* 🌡️ Real-time temperature & humidity monitoring (DHT11)
* 💨 Smoke detection using MQ2 sensor
* 🔥 Flame detection using flame sensor
* ☁️ Live data storage using Firebase Realtime Database
* 🌐 Web dashboard to visualize sensor data
* 📲 Telegram notifications for instant alerts

---

## 🧠 System Architecture

ESP32 (Sensors) → Firebase Realtime Database → Web Dashboard + Telegram Alerts

---

## 🔧 Hardware Components

* ESP32
* DHT11 Sensor
* MQ2 Gas Sensor
* Flame Sensor

---

## 💻 Arduino Code (ESP32)

The Arduino code:

* Reads data from DHT11, MQ2, and flame sensors
* Processes fire risk conditions
* Sends real-time data to Firebase
* Triggers alerts when thresholds are exceeded

📁 File: `forest-fire-project.ino`

---

## ☁️ Firebase Integration

This project uses Firebase Realtime Database as the backend to:

* Store sensor data in real time
* Sync data with the web dashboard
* Enable remote monitoring

### 🔗 Live Database Preview

https://console.firebase.google.com/u/1/project/forest-fire-05/database/forest-fire-05-default-rtdb/data

---

## 🌐 Web Dashboard

A simple web interface to monitor live sensor data.

### Features:

* Displays temperature, humidity, smoke, and flame status
* Updates data in real time from Firebase

📁 File: `dashboard.html`

---

## 📲 Telegram Notifications

The system sends instant alerts via Telegram when:

* Smoke level is high
* Flame is detected
* Temperature exceeds threshold

### How it works:

* ESP32 sends HTTP request to Telegram Bot API
* Message is delivered to user’s Telegram chat

---

## ⚙️ Setup Instructions

### 1. Arduino Setup

* Install required libraries in Arduino IDE
* Connect ESP32 and sensors properly
* Upload the `.ino` code

### 2. Firebase Setup

* Create a Firebase project
* Enable Realtime Database
* Replace Firebase config in your project

### 3. Web Dashboard

* Open `dashboard.html` in browser
* Connect it with Firebase config

### 4. Telegram Bot Setup

* Create a bot using BotFather
* Get Bot Token and Chat ID
* Add them in Arduino code

---

## 🛠️ Tech Stack

* Arduino (ESP32)
* Firebase Realtime Database
* HTML, CSS, JavaScript
* Telegram Bot API

---

## 📌 Future Improvements

* Add mobile app interface
* Improve UI/UX of dashboard
* Add AI-based fire prediction
* Deploy system for large-scale monitoring

---

## 👩‍💻 Author

Khazi Afifa Fathima

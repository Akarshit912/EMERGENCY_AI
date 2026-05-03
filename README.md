# EMERGENCY_AI
ESP32-Based IoT Forest Fire Prediction System

Project

Designed and implemented an IoT-based early forest fire detection system using ESP32 microcontroller for real-time environmental monitoring.
Integrated sensors (temperature, humidity, smoke/gas – e.g., MQ-2/MQ-135) to capture critical parameters influencing fire risk.
Developed a data acquisition and transmission pipeline using Wi-Fi (MQTT/HTTP) to send live sensor data to a cloud platform .
Built a predictive model using Logistic Regression / Decision Tree / Random Forest to classify fire risk levels based on sensor inputs.
Achieved 90% accuracy in fire prediction on test dataset.
Implemented real-time alerts via Blynk and buzzer when risk threshold exceeded.
Optimized power consumption for continuous field deployment using deep sleep modes of ESP32.

Tech Stack: ESP32, Embedded C/C++, IoT Protocols, Sensors (DHT11/22), Python (for ML), Cloud, Data Visualization

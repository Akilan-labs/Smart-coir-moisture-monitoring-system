# 🌱 Automated Coir Moisture Monitoring and Control System

> An IoT-enabled embedded system for intelligent moisture management in coir-based cultivation, built using PIC16F877A and ESP32 microcontrollers.

---

## 📌 Overview

This project presents an automated device designed to monitor and maintain the optimal moisture level of coir substrates in real time. By integrating sensor feedback with automated pump control and IoT cloud connectivity, the system prevents over-irrigation, groundwater seepage, and fertilizer leaching — making coir cultivation more sustainable and efficient.

---

## 🧰 Hardware Components

| Component | Purpose |
|---|---|
| PIC16F877A Microcontroller | Primary control unit — reads sensors, drives relay and LCD |
| ESP32 Microcontroller | IoT gateway — sends data to ThingSpeak via Wi-Fi |
| Capacitive Moisture Sensor | Measures volumetric water content of coir |
| DHT11 Sensor | Measures ambient temperature and humidity |
| 16×2 LCD Display | Local real-time display of moisture, temperature, system status |
| 5V Relay Module | Switches the water pump on/off |
| DC Water Pump / Motor | Delivers water to coir when threshold is breached |

---

## ⚙️ System Architecture

```
[Capacitive Moisture Sensor] ──┐
[DHT11 Sensor]                 ├──► [PIC16F877A] ──► [Relay] ──► [Water Pump]
[Push Buttons (threshold)]     ┘         │
                                         ├──► [16×2 LCD]
                                         └──► [ESP32] ──► [ThingSpeak Cloud]
```

---

## 🔄 Working Principle

1. **Sensing** — The PIC16F877A continuously reads the analog output of the capacitive moisture sensor via its 10-bit ADC and converts it to a moisture percentage using two-point calibration.
2. **Control Logic** — A threshold-based algorithm is implemented:
   - If moisture < **MAD (Maximum Allowable Depletion, ~40%)** → Relay turns ON → Pump starts.
   - If moisture ≥ **FC (Field Capacity, ~80%)** → Relay turns OFF → Pump stops.
3. **Local Display** — Moisture %, temperature, and humidity are shown on the 16×2 LCD in real time.
4. **IoT Monitoring** — The ESP32 receives data from PIC via UART and uploads it to the **ThingSpeak** cloud platform for remote monitoring and historical logging.

---

## 🖥️ Simulation

The circuit was designed and verified in **Proteus Design Suite** before physical assembly:

- A potentiometer was used to simulate the analog moisture sensor output.
- The compiled `.hex` firmware was loaded into the virtual PIC16F877A.
- The control algorithm and LCD output were validated in simulation.


---


## 🚀 Getting Started

### Prerequisites
- **MikroC Pro for PIC** — to compile PIC firmware
- **Arduino IDE** with ESP32 board support — to flash ESP32
- **Proteus Design Suite** — to open simulation files
- **MPLAB IPE** or **PicKit3** — to flash PIC microcontroller


---

## 📊 Results

| Test | Observation |
|---|---|
| Simulation (Proteus) | LCD correctly displayed moisture %; relay triggered at < 40% threshold |
| Hardware Prototype | Closed-loop control maintained moisture between 40%–80% automatically |
| IoT Logging | ThingSpeak dashboard plotted moisture % over time in real time |

---

## 🔮 Future Scope

- Add **EC (Electrical Conductivity)** and **pH** sensors for comprehensive medium analysis.
- Use **LoRaWAN** for connectivity in remote/rural areas without Wi-Fi.
- Integrate **ML-based irrigation prediction** using historical data and weather forecasts.
- Develop a **dedicated mobile app** for remote monitoring and threshold control.

---

## 📄 Report

The full project report is available in report pdf

---

## 📜 License

This project is submitted as an academic project. All rights reserved by the authors.

# 🚍 IoT-Based RFID Shuttle Payment System

An IoT-powered RFID payment and monitoring system designed for university shuttle services and transport networks. This project uses an **ESP32**, **RC522 RFID reader**, and **Firebase Realtime Database** to enable secure, contactless fare payments in real-time.

---

## 📌 Features

- ✅ RFID card scanning and UID verification  
- 💰 Real-time fare deduction and balance update via Firebase  
- 🔐 Unique identification with Email, UID, and HashKey  
- 🔢 Fare input using a 4x4 matrix keypad  
- 📺 LCD2004 I2C display for user feedback and card info  
- 🔔 Buzzer and LED indicators for success/failure status  
- 📤 Email notification to users on successful transactions  
- 🚫 Automatic rejection of invalid or insufficient-balance cards  

---

## 🧰 Components Used

- ESP32 Dev Module (38 Pins)  
- RC522 RFID Reader (13.56 MHz)  
- 4x4 Matrix Keypad  
- LCD2004 I2C Display  
- Firebase Realtime Database  
- Buzzer  
- 4x LED indicators (Power, Wi-Fi, Accepted, Rejected)  
- 18650 Li-ion Batteries (x4, in parallel)  
- Boost Converter (XL6019)  
- Charging Module with LCD  
- Type-C Charging Port  
- ON/OFF Switch
 
---

## 🔧 How It Works

### 🏠 Homepage Options on LCD:
- **A: Card Information**  
  Displays name, current balance, previous balance, and last fare.

- **\*: Make Payment**  
  Prompts for fare entry, confirms payment with `#`, clears with `C`, cancels with `D`.  
  Checks if balance ≥ fare, deducts from Firebase, and sends an email notification.

---

## 📽️ Demo Video

🎥 **Watch it in action on YouTube**:  
👉 [Click here to watch the full demo](https://www.youtube.com/watch?v=3elCyCiH2AM)

---




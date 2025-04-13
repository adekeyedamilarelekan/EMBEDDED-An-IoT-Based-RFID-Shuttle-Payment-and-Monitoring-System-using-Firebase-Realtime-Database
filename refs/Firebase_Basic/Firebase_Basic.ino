
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP32
 *
 * Copyright (c) 2023 mobizt
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// /* 1. Define the WiFi credentials */
// #define WIFI_SSID "Airtel_X25A_197F"
// #define WIFI_PASSWORD "F270F827"

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Deewan"
#define WIFI_PASSWORD "vvvvgggg"

// define the GPIO Pins
#define WiFi_Led 13

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// unsigned long sendDataPrevMillis = 0;
unsigned long readDataPrevMillis = 0;

unsigned long Current_Account_Balance = 1000;
unsigned long Initial_Account_Balance = 1200;
unsigned long Last_Vehicle_Cost = 200;

String student_Email = "rfidprojectokoro@gmail.com";
String student_Matric = "20CK027348";
String student_Name = "Okoro Kelly Efedhoma";
String student_RFID_UID = "83 66 78 29";
String student_HashKey = "CU-001";
String Driver_Name = "Damilare Lekan Adekeye";
String Driver_Email = "adekeyedamilarelekan@gmail.com";

void setup() {
  pinMode(WiFi_Led, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WiFi_Led, HIGH);  // turn the LED on (HIGH is the voltage level)
    Serial.println("\nConnected to WiFi");
  } else {
    digitalWrite(WiFi_Led, LOW);  // turn the LED off (LOW is the voltage level)
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);


  // Or use legacy authenticate method
  config.database_url = "https://rfidtagpaymentproject-default-rtdb.firebaseio.com";
  config.signer.tokens.legacy_token = "ARaRzz1UTSgtatGcCGr93gkzyB6KCiOGL0WZttf0";

  // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  // otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.setDoubleDigits(5);

  Firebase.signUp(&config, &auth, "", "");  // Anonymous sign-in
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {

  //this example for sending data is working.
  // if (Firebase.ready() && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
  //   sendDataPrevMillis = millis();
  //   FirebaseJson DriverData;
  //   DriverData.set("Driver Email", Driver_Email);
  //   DriverData.set("Driver Name", Driver_Name);

  //   FirebaseJson studentData;
  //   studentData.set("Name", student_Name);
  //   studentData.set("Matric Number", student_Matric);
  //   studentData.set("HashKey", student_HashKey);
  //   studentData.set("Email", student_Email);
  //   studentData.set("UID", student_RFID_UID);
  //   studentData.set("/Account Balance/Current Balance", Current_Account_Balance);
  //   studentData.set("/Account Balance/Initial Balance", Initial_Account_Balance);
  //   studentData.set("/Account Balance/Last Updated Vehicle Cost", Last_Vehicle_Cost);


  //   // Firebase.set(fbdo, F("/Student/studentA/"), studentData);
  //   // Firebase.set(fbdo, F("/Student/studentB/"), studentData);
  //   // Firebase.set(fbdo, F("/Student/studentC/"), studentData);
  //   // Firebase.set(fbdo, F("/Student/studentD/"), studentData);
  //   // Firebase.set(fbdo, F("/Driver/"), DriverData);

  //   // Serial.printf("Updating Student Data... %s\n", Firebase.set(fbdo, F("/Student/studentA/"), studentData) ? "Success" : fbdo.errorReason().c_str());
  //   // Serial.printf("Updating Drivers Data... %s\n", Firebase.set(fbdo, F("/Driver/"), DriverData) ? "Success" : fbdo.errorReason().c_str());
  //   Serial.println();
  // }

  // New data reading code
  if (Firebase.ready() && (millis() - readDataPrevMillis > 2000 || readDataPrevMillis == 0)) {
    readDataPrevMillis = millis();

    Serial.println("\n------- READING DATA -------");

    // Read full driver data
    if (Firebase.getJSON(fbdo, "/Driver")) {
      FirebaseJson driverJson = fbdo.to<FirebaseJson>();
      FirebaseJsonData driverResult;

      if (driverJson.get(driverResult, "Driver Name")) {
        Serial.println("Driver Name: " + driverResult.to<String>());
      }
      if (driverJson.get(driverResult, "Driver Email")) {
        Serial.println("Driver Email: " + driverResult.to<String>());
      }
    }
    Serial.println("---------------------------\n");

    // Read full driver data for studentA
    if (Firebase.getJSON(fbdo, "/Student/studentA")) {
      FirebaseJson studentJson = fbdo.to<FirebaseJson>();
      FirebaseJsonData studentResult;
      if (studentJson.get(studentResult, "/Name/")) {
        Serial.println("Name: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Matric Number/")) {
        Serial.println("Matric Number: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Email/")) {
        Serial.println("Email: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/UID/")) {
        Serial.println("UID: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Current Balance")) {
        Serial.println("Current Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Initial Balance")) {
        Serial.println("Initial Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Last Updated Vehicle Cost")) {
        Serial.println("Last Updated Vehicle Cost: " + studentResult.to<String>());
      }
    }
    Serial.println("---------------------------\n");

    // Read full driver data for studentB
    if (Firebase.getJSON(fbdo, "/Student/studentB")) {
      FirebaseJson studentJson = fbdo.to<FirebaseJson>();
      FirebaseJsonData studentResult;
      if (studentJson.get(studentResult, "/Name/")) {
        Serial.println("Name: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Matric Number/")) {
        Serial.println("Matric Number: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Email/")) {
        Serial.println("Email: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/UID/")) {
        Serial.println("UID: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Current Balance")) {
        Serial.println("Current Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Initial Balance")) {
        Serial.println("Initial Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Last Updated Vehicle Cost")) {
        Serial.println("Last Updated Vehicle Cost: " + studentResult.to<String>());
      }
    }
    Serial.println("---------------------------\n");

    // Read full driver data for studentC
    if (Firebase.getJSON(fbdo, "/Student/studentC")) {
      FirebaseJson studentJson = fbdo.to<FirebaseJson>();
      FirebaseJsonData studentResult;
      if (studentJson.get(studentResult, "/Name/")) {
        Serial.println("Name: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Matric Number/")) {
        Serial.println("Matric Number: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Email/")) {
        Serial.println("Email: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/UID/")) {
        Serial.println("UID: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Current Balance")) {
        Serial.println("Current Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Initial Balance")) {
        Serial.println("Initial Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Last Updated Vehicle Cost")) {
        Serial.println("Last Updated Vehicle Cost: " + studentResult.to<String>());
      }
    }
    Serial.println("---------------------------\n");

    // Read full driver data for studentD
    if (Firebase.getJSON(fbdo, "/Student/studentD")) {
      FirebaseJson studentJson = fbdo.to<FirebaseJson>();
      FirebaseJsonData studentResult;
      if (studentJson.get(studentResult, "/Name/")) {
        Serial.println("Name: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Matric Number/")) {
        Serial.println("Matric Number: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Email/")) {
        Serial.println("Email: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/UID/")) {
        Serial.println("UID: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Current Balance")) {
        Serial.println("Current Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Initial Balance")) {
        Serial.println("Initial Balance: " + studentResult.to<String>());
      }
      if (studentJson.get(studentResult, "/Account Balance/Last Updated Vehicle Cost")) {
        Serial.println("Last Updated Vehicle Cost: " + studentResult.to<String>());
      }
    }
    Serial.println("---------------------------\n");
  }
}

// // Firebase.ready() should be called repeatedly to handle authentication tasks.

// if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
//   sendDataPrevMillis = millis();

//   Serial.printf("Set bool... %s\n", Firebase.setBool(fbdo, F("/test/bool"), count % 2 == 0) ? "ok" : fbdo.errorReason().c_str());

//   Serial.printf("Get bool... %s\n", Firebase.getBool(fbdo, FPSTR("/test/bool")) ? fbdo.to<bool>() ? "true" : "false" : fbdo.errorReason().c_str());

//   bool bVal;
//   Serial.printf("Get bool ref... %s\n", Firebase.getBool(fbdo, F("/test/bool"), &bVal) ? bVal ? "true" : "false" : fbdo.errorReason().c_str());

//   Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/test/int"), count) ? "ok" : fbdo.errorReason().c_str());

//   Serial.printf("Get int... %s\n", Firebase.getInt(fbdo, F("/test/int")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());

//   int iVal = 0;
//   Serial.printf("Get int ref... %s\n", Firebase.getInt(fbdo, F("/test/int"), &iVal) ? String(iVal).c_str() : fbdo.errorReason().c_str());

//   Serial.printf("Set float... %s\n", Firebase.setFloat(fbdo, F("/test/float"), count + 10.2) ? "ok" : fbdo.errorReason().c_str());

//   Serial.printf("Get float... %s\n", Firebase.getFloat(fbdo, F("/test/float")) ? String(fbdo.to<float>()).c_str() : fbdo.errorReason().c_str());

//   Serial.printf("Set double... %s\n", Firebase.setDouble(fbdo, F("/test/double"), count + 35.517549723765) ? "ok" : fbdo.errorReason().c_str());

//   Serial.printf("Get double... %s\n", Firebase.getDouble(fbdo, F("/test/double")) ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());

//   Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("/test/string"), "Hello World!") ? "ok" : fbdo.errorReason().c_str());

//   Serial.printf("Get string... %s\n", Firebase.getString(fbdo, F("/test/string")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());

//   // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino
//   FirebaseJson json;

//   if (count == 0) {
//     json.set("value/round/" + String(count), F("cool!"));
//     json.set("value/round/" + String(count), F("cool!"));
//     json.set(F("vaue/ts/.sv"), F("timestamp"));
//     Serial.printf("Set json... %s\n", Firebase.set(fbdo, F("/test/json"), json) ? "ok" : fbdo.errorReason().c_str());
//   } else {
//     json.add(String(count), "smart!");
//     Serial.printf("Update node... %s\n", Firebase.updateNode(fbdo, F("/test/json/value/round"), json) ? "ok" : fbdo.errorReason().c_str());
//   }

//   Serial.println();

// For generic set/get functions.

// For generic set, use Firebase.set(fbdo, <path>, <any variable or value>)

// For generic get, use Firebase.get(fbdo, <path>).
// And check its type with fbdo.dataType() or fbdo.dataTypeEnum() and
// cast the value from it e.g. fbdo.to<int>(), fbdo.to<std::string>().

// The function, fbdo.dataType() returns types String e.g. string, boolean,
// int, float, double, json, array, blob, file and null.

// The function, fbdo.dataTypeEnum() returns type enum (number) e.g. firebase_rtdb_data_type_null (1),
// firebase_rtdb_data_type_integer, firebase_rtdb_data_type_float, firebase_rtdb_data_type_double,
// firebase_rtdb_data_type_boolean, firebase_rtdb_data_type_string, firebase_rtdb_data_type_json,
// firebase_rtdb_data_type_array, firebase_rtdb_data_type_blob, and firebase_rtdb_data_type_file (10)

//   count++;
// }

/// PLEASE AVOID THIS ////

// Please avoid the following inappropriate and inefficient use cases
/**
 *
 * 1. Call get repeatedly inside the loop without the appropriate timing for execution provided e.g. millis() or conditional checking,
 * where delay should be avoided.
 *
 * Everytime get was called, the request header need to be sent to server which its size depends on the authentication method used,
 * and costs your data usage.
 *
 * Please use stream function instead for this use case.
 *
 * 2. Using the single FirebaseData object to call different type functions as above example without the appropriate
 * timing for execution provided in the loop i.e., repeatedly switching call between get and set functions.
 *
 * In addition to costs the data usage, the delay will be involved as the session needs to be closed and opened too often
 * due to the HTTP method (GET, PUT, POST, PATCH and DELETE) was changed in the incoming request.
 *
 *
 * Please reduce the use of swithing calls by store the multiple values to the JSON object and store it once on the database.
 *
 * Or calling continuously "set" or "setAsync" functions without "get" called in between, and calling get continuously without set
 * called in between.
 *
 * If you needed to call arbitrary "get" and "set" based on condition or event, use another FirebaseData object to avoid the session
 * closing and reopening.
 *
 * 3. Use of delay or hidden delay or blocking operation to wait for hardware ready in the third party sensor libraries, together with stream functions e.g. Firebase.RTDB.readStream and fbdo.streamAvailable in the loop.
 *
 * Please use non-blocking mode of sensor libraries (if available) or use millis instead of delay in your code.
 *
 * 4. Blocking the token generation process.
 *
 * Let the authentication token generation to run without blocking, the following code MUST BE AVOIDED.
 *
 * while (!Firebase.ready()) <---- Don't do this in while loop
 * {
 *     delay(1000);
 * }
 *
 */
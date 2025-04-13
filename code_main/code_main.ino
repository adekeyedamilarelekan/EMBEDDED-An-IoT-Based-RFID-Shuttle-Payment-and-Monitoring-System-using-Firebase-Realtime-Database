#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <HTTPClient.h>  //for email

#define SS_PIN 5
#define RST_PIN 0

/* 1. Define the WiFi credentials */
#define WIFI_SSID "N**************t"
#define WIFI_PASSWORD "vv*****vv"

// Mailjet API credentials
const char* apiKey = "949************************b8967c";
const char* secretKey = "0b397*********************bbb60a";
const char* senderEmail = "rfidprojectokoro@gmail.com";  // e.g., yourname@yourdomain.com


// define the GPIO Pins
#define buzzer 27
#define RED_Led 12
#define BLUE_Led 4
#define WiFi_Led 13
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  { '1', '4', '7', '*' },
  { '2', '5', '8', '0' },
  { '3', '6', '9', '#' },
  { 'A', 'B', 'C', 'D' }
};
byte rowPins[ROWS] = { 32, 33, 25, 26 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 17, 16, 2, 15 };   //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

bool enterMode = false;
bool scanCardModeForPayment = false;
bool scanCardModeForCardInfo = false;
bool showHomePage_true_false = true;
bool updateFirebaseData_true_false = true;

// Global variables to store student details
String studentName, matricNumber, email, studentUID, hashKey;
float currentBalance, initialBalance, lastUpdatedVehicleCost;

String paymentPrice, STUDENT, Driver_Name, messageBody;

unsigned long newBalance, enteredAmount, previousMillis_WiFi_Check;

void setup() {
  Serial.begin(115200);  // Initiate a serial communication
  SPI.begin();           // Initiate  SPI bus
  mfrc522.PCD_Init();    // Initiate MFRC522

  // declare output terminals.
  pinMode(WiFi_Led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(RED_Led, OUTPUT);
  pinMode(BLUE_Led, OUTPUT);

  // initialize the lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID UNIVERSAL TICKE");
  lcd.setCursor(0, 1);
  lcd.print("-TING SYSTEM IN THE ");
  lcd.setCursor(0, 2);
  lcd.print("******** UNIVERSITY ");
  lcd.setCursor(0, 3);
  lcd.print("SHUTTLE SERVICES.   ");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to Wi-Fi ");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  delay(3000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to Wi-Fi"));

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("."));
    delay(300);
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WiFi_Led, HIGH);  // turn the LED on (HIGH is the voltage level)
    Serial.println(F("\nConnected to WiFi"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wi-Fi Connected.");
    delay(1000);
  } else {
    digitalWrite(WiFi_Led, LOW);  // turn the LED off (LOW is the voltage level)
  }
  Serial.print(F("Connected with IP: "));
  Serial.println(WiFi.localIP());

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // You can use TCP KeepAlive in FirebaseData object and tracking the server connection status, please read this for detail.
  // https://github.com/mobizt/Firebase-ESP32#about-firebasedata-object
  fbdo.keepAlive(5, 5, 1);

  // Or use legacy authenticate method
  config.database_url = "https://rfidtagpaymentproject-default-rtdb.firebaseio.com";
  config.signer.tokens.legacy_token = "ARaRz*********************************f0";

  //Network reconnect timeout (interval) in ms (10 sec - 5 min) when network or WiFi disconnected.
  config.timeout.networkReconnect = 10 * 1000;

  // //Socket connection and SSL handshake timeout in ms (1 sec - 1 min).
  // config.timeout.socketConnection = 10 * 1000;
  Firebase.setDoubleDigits(5);

  Firebase.signUp(&config, &auth, "", "");  // Anonymous sign-in
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // display information for LCD on Email.
  lcd.setCursor(0, 2);
  lcd.print("Configuring Mail");

  // /*  Set the network reconnection option */
  // MailClient.networkReconnect(true);
  // smtp.debug(1);
  // /* Set the callback function to get the sending results */
  // smtp.callback(smtpCallback);

  lcd.clear();
  showHomePage();
}

void loop() {

  //check WiFi every 5 secs without delay().
  if (millis() - previousMillis_WiFi_Check >= 5000) {
    previousMillis_WiFi_Check = millis();  // Update last execution time

    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(WiFi_Led, HIGH);  // Turn LED on
    } else {
      digitalWrite(WiFi_Led, LOW);  // Turn LED off
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.println(F("WiFi disconnected. Reconnecting..."));

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wi-Fi Disconnected.");
      lcd.setCursor(0, 1);
      lcd.print("Operations halted.  ");
      lcd.setCursor(0, 2);
      lcd.print("Reconnecting...    ");
      lcd.setCursor(0, 3);
      lcd.print("Please turn Wi-Fi ON");
      // delay(3000);
    }
  }

  // Run All processes when Wi-Fi is only connected.
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WiFi_Led, HIGH);  // Turn LED on
    if (showHomePage_true_false)
      showHomePage();
    char customKey = customKeypad.getKey();
    if (customKey) {
      // Serial.println(customKey);
      handleKeyInput(customKey);
    }
    delay(50);  //  delay to avoid debounce

    if (scanCardModeForPayment)  //if this is true then let the card scanning process begin for payment.
      proceedToScanCardForPayment();

    if (scanCardModeForCardInfo)  //if this is true then let the card scanning process begin to check card info.
      proceedToScanCardForCardInfo();
    // showHomePage();
  }
}

void proceedToScanCardForPayment() {
  // Call scan_card() and store the returned UID
  String UID = scan_card();

  // Print the UID to the Serial Monitor
  if (UID != "") {               //if UID is fetched.
    digitalWrite(buzzer, HIGH);  // turn the Buzzer on (HIGH is the voltage level)
    delay(300);
    digitalWrite(buzzer, LOW);  // turn the Buzzer on (HIGH is the voltage level)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card  Detected!");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Starting Transaction");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print("Txn in Progress...");
    delay(1000);
    lcd.setCursor(0, 3);
    lcd.print("Please wait...");

    //pass UID into readStudentByUID function.
    // Serial.println("Scanned UID: " + UID);
    String userDetails = readStudentByUID(UID);  // Store the result
    Serial.println(userDetails);                 // Print the result or use it elsewhere
    if (userDetails != "No user found") {
      digitalWrite(BLUE_Led, HIGH);
      digitalWrite(RED_Led, LOW);
      // Function to extract the student details from the response (userDetails)/
      extractStudentDetails(userDetails);  // Extract and process the details
      lcd.clear();
      processPayment(currentBalance);
      lcd.clear();
    }

    else {
      digitalWrite(BLUE_Led, LOW);
      digitalWrite(RED_Led, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card not Registered.");
      lcd.setCursor(0, 2);
      lcd.print("Please register your");
      lcd.setCursor(0, 3);
      lcd.print("card to allow Txn.  ");
      delay(3000);
      lcd.clear();
      digitalWrite(BLUE_Led, LOW);
      digitalWrite(RED_Led, LOW);
      // all variable to return to the homepage and reset the whole condition should be here.
      scanCardModeForPayment = false;
      showHomePage_true_false = true;  //make homepage display.
      enterMode = false;
    }
  }

  else {  //if no UID is fetched yet!
    digitalWrite(BLUE_Led, LOW);
    digitalWrite(RED_Led, LOW);
  }
}

void proceedToScanCardForCardInfo() {
  // Call scan_card() and store the returned UID
  String UID = scan_card();

  // Print the UID to the Serial Monitor
  if (UID != "") {               //if UID is fetched.
    digitalWrite(buzzer, HIGH);  // turn the Buzzer oprocessPaymentn (HIGH is the voltage level)
    delay(300);
    digitalWrite(buzzer, LOW);  // turn the Buzzer on (HIGH is the voltage level)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Detected!");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Checking Card Info");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print("Please wait...");

    //pass UID into readStudentByUID function.
    // Serial.println("Scanned UID: " + UID);
    String userDetails = readStudentByUID(UID);  // Store the result
    Serial.println(userDetails);                 // Print the result or use it elsewhere
    if (userDetails != "No user found") {
      digitalWrite(BLUE_Led, HIGH);
      digitalWrite(RED_Led, LOW);
      // Function to extract the student details from the response (userDetails)/
      extractStudentDetails(userDetails);  // Extract and process the details
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(studentName);
      lcd.setCursor(0, 1);
      lcd.print("Card Balance: ");
      lcd.print(int(currentBalance));
      lcd.setCursor(0, 2);
      lcd.print("Prev Balance: ");
      lcd.print(int(initialBalance));
      lcd.setCursor(0, 3);
      lcd.print("Last Debit: ");
      lcd.print(int(lastUpdatedVehicleCost));
      delay(5000);
      lcd.clear();
      digitalWrite(BLUE_Led, LOW);
      digitalWrite(RED_Led, LOW);
      // all variable to return to the homepage and reset the whole condition should be here.
      scanCardModeForCardInfo = false;
      showHomePage_true_false = true;  //make homepage display.
      enterMode = false;
    }

    else {
      digitalWrite(BLUE_Led, LOW);
      digitalWrite(RED_Led, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card not Registered.");
      lcd.setCursor(0, 2);
      lcd.print("Please register card");
      lcd.setCursor(0, 3);
      lcd.print("to allow info check.");
      delay(3000);
      lcd.clear();
      digitalWrite(BLUE_Led, LOW);
      digitalWrite(RED_Led, LOW);
      // all variable to return to the homepage and reset the whole condition should be here.
      scanCardModeForCardInfo = false;
      showHomePage_true_false = true;  //make homepage display.
      enterMode = false;
    }
  }

  else {  //if no UID is fetched yet!
    digitalWrite(BLUE_Led, LOW);
    digitalWrite(RED_Led, LOW);
  }
}

void showHomePage() {
  lcd.setCursor(0, 0);
  lcd.print("* = Make Payment");
  lcd.setCursor(0, 2);
  lcd.print("A = Card Information");
}


void handleKeyInput(char key) {
  if (!enterMode) {
    if (key == '*') {
      showHomePage_true_false = false;  //make homepage not display.
      enterMode = true;
      paymentPrice = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Amount: ");
      lcd.setCursor(0, 3);
      lcd.print("C: Clear, D: Cancel");
    } else if (key == 'A') {
      scanCardModeForCardInfo = true;
      showHomePage_true_false = false;  //make homepage display.
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Scan Card to Fetch");
      lcd.setCursor(0, 1);
      lcd.print("Information.");
      lcd.setCursor(0, 3);
      lcd.print("Waiting...");
    }
  }

  else {
    if (isdigit(key)) {
      paymentPrice += key;
      lcd.setCursor(0, 1);
      lcd.print(paymentPrice);
      if (paymentPrice.length() == 3) {
        lcd.setCursor(0, 2);
        lcd.print("#: Confirm/Enter");
      }
    } else if (key == 'C') {
      paymentPrice = "";
      lcd.setCursor(0, 1);
      lcd.print("                    ");
    } else if (key == '#') {
      scanCardModeForPayment = true;
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Scan Card to Pay");
      lcd.setCursor(0, 2);
      lcd.print("Waiting...");
    } else if (key == 'D' && scanCardModeForPayment == false) {
      enterMode = false;
      showHomePage_true_false = true;  //make homepage display.
      scanCardModeForPayment = false;
      lcd.clear();
      showHomePage();
    }
  }
}


void processPayment(float firebaseBalance) {
  enteredAmount = paymentPrice.toInt();
  if (firebaseBalance >= enteredAmount) {
    newBalance = 0;
    newBalance = firebaseBalance - enteredAmount;
    Serial.println(F("Payment Successful"));
    Serial.print(F("New Balance: "));
    Serial.println(newBalance);
    lcd.setCursor(0, 0);
    lcd.print("Payment Successful!");
    lcd.setCursor(0, 1);
    lcd.print("Debit: " + String(enteredAmount));
    lcd.setCursor(0, 2);
    lcd.print("From:");
    lcd.setCursor(0, 3);
    lcd.print(studentName);
    updateFirebaseData();
    delay(2000);
    digitalWrite(BLUE_Led, LOW);  //Off the LED indicator.
    sendEmail(email);             //send payment information to the respective emails.

    // all variable to return to the homepage and reset the whole condition should be here.
    scanCardModeForPayment = false;
    showHomePage_true_false = true;  //make homepage display.
    enterMode = false;
  } else {
    Serial.println(F("Insufficient Funds"));
    lcd.setCursor(0, 0);
    lcd.print("Insufficient Funds");
    delay(3000);
    digitalWrite(BLUE_Led, LOW);  //Off the LED indicator.

    // all variable to return to the homepage and reset the whole condition should be here.
    scanCardModeForPayment = false;
    showHomePage_true_false = true;  //make homepage display.
    enterMode = false;
  }
}

//Function to scan ID Card.
String scan_card() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return "";  // No card detected
  }
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  return content.substring(1);
}

String readStudentByUID(const String& targetUID) {

  // Read driver data
  if (Firebase.getJSON(fbdo, "/Driver")) {
    FirebaseJson driverJson = fbdo.to<FirebaseJson>();
    FirebaseJsonData driverResult;

    if (driverJson.get(driverResult, "Driver Name")) {
      Serial.println("Driver Name: " + driverResult.to<String>());
      Driver_Name = "";
      Driver_Name = "** Shuttle Driver: " + driverResult.to<String>();
    }
    if (driverJson.get(driverResult, "Driver Email")) {
      Serial.println("Driver Email: " + driverResult.to<String>());
    }
  }

  // Read student details.
  String students[] = { "studentA", "studentB", "studentC", "studentD" };

  for (String student : students) {
    String path = "/Student/" + student;

    if (Firebase.getJSON(fbdo, path)) {
      FirebaseJson studentJson = fbdo.to<FirebaseJson>();
      FirebaseJsonData studentResult;

      // Check if UID exists in JSON
      if (studentJson.get(studentResult, "UID")) {
        String foundUID = studentResult.to<String>();

        if (foundUID == targetUID) {  // If UID matches, return details
          STUDENT = "";
          STUDENT = student;
          String studentInfo = "\n--- Student Found (" + student + ") ---\n";

          String fields[] = { "Name", "Matric Number", "Email", "UID", "HashKey",
                              "Account Balance/Current Balance",
                              "Account Balance/Initial Balance",
                              "Account Balance/Last Updated Vehicle Cost" };

          for (String field : fields) {
            if (studentJson.get(studentResult, field)) {
              studentInfo += field + ": " + studentResult.to<String>() + "\n";
            }
          }

          studentInfo += "-------------------------------------------------\n";
          return studentInfo;  // Return student details if found
        }
      }
    }
  }

  return "No user found";  // Return this ONLY if no UID matches after checking all students
}


void extractStudentDetails(const String& userDetails) {
  if (userDetails.startsWith("No user found")) {
    Serial.println(F("No user found"));
    return;
  }

  // Split the string and extract values
  int startIndex = 0;
  int endIndex = userDetails.indexOf("\n", startIndex);

  while (endIndex != -1) {
    String line = userDetails.substring(startIndex, endIndex);
    // Serial.println("Debugging Line: " + line);  // Print each line for debugging

    if (line.startsWith("Name: ")) {
      studentName = line.substring(6);
    } else if (line.startsWith("Matric Number: ")) {
      matricNumber = line.substring(15);
    } else if (line.startsWith("Email: ")) {
      email = line.substring(7);
    } else if (line.startsWith("UID: ")) {
      studentUID = line.substring(5);
    } else if (line.startsWith("HashKey: ")) {
      hashKey = line.substring(9);
    } else if (line.startsWith("Account Balance/Current Balance: ")) {
      currentBalance = line.substring(33).toFloat();
    } else if (line.startsWith("Account Balance/Initial Balance: ")) {
      initialBalance = line.substring(33).toFloat();
    } else if (line.startsWith("Account Balance/Last Updated Vehicle Cost: ")) {
      String valueStr = line.substring(line.indexOf(":") + 2);  // Extract value
      valueStr.trim();                                          // Remove any extra spaces
      lastUpdatedVehicleCost = valueStr.toFloat();              // Convert to float
    }
    startIndex = endIndex + 1;
    endIndex = userDetails.indexOf("\n", startIndex);
  }

  // // Print extracted details
  // Serial.println("\n--- Extracted Student Details ---");
  // Serial.println("Name: " + studentName);
  // Serial.println("Matric Number: " + matricNumber);
  // Serial.println("Email: " + email);
  // Serial.println("HashKey: " + hashKey);
  // Serial.println("UID: " + studentUID);
  // Serial.print("Current Balance: ");
  // Serial.println(currentBalance);
  // Serial.print("Initial Balance: ");
  // Serial.println(initialBalance);
  // Serial.print("Last Updated Vehicle Cost: ");
  // Serial.println(lastUpdatedVehicleCost);
  // Serial.println("------------------------------");
}

//update firebase data
void updateFirebaseData() {
  updateFirebaseData_true_false = true;
  // if (Firebase.ready() && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
  // sendDataPrevMillis = millis();
  if (Firebase.ready() && updateFirebaseData_true_false) {
    // FirebaseJson DriverData;
    // DriverData.set("Driver Email", Driver_Email);
    // DriverData.set("Driver Name", Driver_Name);

    FirebaseJson studentData;
    studentData.set("/Account Balance/Current Balance", newBalance);
    studentData.set("/Account Balance/Initial Balance", currentBalance);
    studentData.set("/Account Balance/Last Updated Vehicle Cost", enteredAmount);
    studentData.set("Name", studentName);
    studentData.set("Matric Number", matricNumber);
    studentData.set("HashKey", hashKey);
    studentData.set("Email", email);
    studentData.set("UID", studentUID);

    // Construct the path dynamically
    String path = "/Student/" + STUDENT + "/";
    Serial.printf("Updating Student Data... %s\n", Firebase.set(fbdo, path.c_str(), studentData) ? "Success" : fbdo.errorReason().c_str());
    // Serial.printf("Updating Drivers Data... %s\n", Firebase.set(fbdo, F("/Driver/"), DriverData) ? "Success" : fbdo.errorReason().c_str());
    updateFirebaseData_true_false = false;
  }
}

// the below two functions are for Email sending...
void sendEmail(String toEmail) {
  HTTPClient http;

  // Mailjet API endpoint
  String url = "https://api.mailjet.com/v3.1/send";
  http.begin(url);

  // Set headers
  String auth = "Basic " + base64Encode(String(apiKey) + ":" + String(secretKey));  // Ensure apiKey and secretKey are properly concatenated
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", auth);

  String messageBody = "";
  messageBody += "<b>Name:</b> " + studentName + ".<br>";
  messageBody += "<b>Matric Number:</b> " + matricNumber + ".<br>";
  messageBody += "<b>Email:</b> " + email + ".<br>";
  messageBody += "<b>HashKey:</b> " + hashKey + ".<br>";
  messageBody += "<b>UID:</b> " + studentUID + ".<br>";
  messageBody += "<b>Current Balance:</b> " + String(newBalance) + ".<br>";
  messageBody += "<b>Initial Balance:</b> " + String(currentBalance) + ".<br>";
  messageBody += "<b>Last Updated Vehicle Cost:</b> " + String(enteredAmount) + ".<br>";

  // Build JSON payload step-by-step
  String payload = "{\"Messages\":[";
  payload += "{";
  payload += "\"From\":{\"Email\":\"" + String(senderEmail) + "\",\"Name\":\"" + Driver_Name + "\"},";
  payload += "\"To\":[{\"Email\":\"" + toEmail + "\",\"Name\":\"Recipient\"}],";
  payload += "\"Subject\":\"PAYMENT INFORMATION FOR UNIVERSITY SHUTTLE SERVICES.\",";
  // payload += "\"TextPart\":\"Hello, deewan deewan.!\",";
  payload += "\"HTMLPart\":\"<h3>Payment Information:</h3><p>" + messageBody + "</p>\"";
  payload += "}";
  payload += "]}";

  // Send POST request
  int httpResponseCode = http.POST(payload);

  // Check response
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);

    //display status on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Email Notification");
    lcd.setCursor(0, 1);
    lcd.print("is Sent Successfully");
    delay(3000);
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("http response failed. ");
    lcd.setCursor(0, 1);
    lcd.print("Error Sending Email");
    lcd.setCursor(0, 2);
    lcd.print("Notification.");
    delay(3000);
  }

  http.end();  // Free resources
}

// Simple Base64 encoding function (you can use a library if preferred)
String base64Encode(String input) {
  const char* base64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String encoded = "";
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  for (i = 0; i < input.length(); i++) {
    char_array_3[j++] = input[i];
    if (j == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (j = 0; j < 4; j++) {
        encoded += base64chars[char_array_4[j]];
      }
      j = 0;
    }
  }

  if (j) {
    for (int k = j; k < 3; k++) {
      char_array_3[k] = '\0';
    }
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (int k = 0; k < j + 1; k++) {
      encoded += base64chars[char_array_4[k]];
    }
    while (j++ < 3) {
      encoded += '=';
    }
  }
  return encoded;
}

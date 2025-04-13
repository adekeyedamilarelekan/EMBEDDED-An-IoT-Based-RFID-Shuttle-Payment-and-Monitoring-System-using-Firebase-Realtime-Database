//
#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "Network_Connect";
const char* password = "vvvvvvvv";

// Mailjet API credentials
const char* apiKey = "94905f636c3fa89c7b0dee62b9b8967c";
const char* secretKey = "0b397acc653cdb418d5eef815abbb60a";
const char* senderEmail = "rfidprojectokoro@gmail.com";  // e.g., yourname@yourdomain.com

// Dynamic recipient email (this can change at runtime)
String recipientEmail = "adekeyedamilarelekan@gmail.com";  // Example initial value
String messageBody = "";
void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  // Example: Update recipientEmail dynamically (e.g., from sensor or input)
  // recipientEmail = "newrecipient@example.com"; // Replace with your logic

  if (WiFi.status() == WL_CONNECTED) {
    sendEmail(recipientEmail);
  } else {
    Serial.println("Wi-Fi disconnected");
  }

  delay(60000);  // Wait 60 seconds before next attempt (adjust as needed)
}

void sendEmail(String toEmail) {
  HTTPClient http;

  // Mailjet API endpoint
  String url = "https://api.mailjet.com/v3.1/send";
  http.begin(url);

  // Set headers
  String auth = "Basic " + base64Encode(String(apiKey) + ":" + String(secretKey));  // Ensure apiKey and secretKey are properly concatenated
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", auth);

  //dummie value for test. Tested and worked.
  String studentName = "Okoro";
  String matricNumber = "20CK4343434";
  String email = "abcdefg@deewansonic.ng";
  String hashKey = "CU-001";
  String studentUID = "45 35 A8 C3";
  String newBalance = "2000";
  String currentBalance = "2200";
  String enteredAmount = "200";
  String messageBody = "";
  messageBody += "<b>Name:</b> " + studentName + ".<br>";
  messageBody += "<b>Matric Number:</b> " + matricNumber + ".<br>";
  messageBody += "<b>Email:</b> " + email + ".<br>";
  messageBody += "<b>HashKey:</b> " + hashKey + ".<br>";
  messageBody += "<b>UID:</b> " + studentUID + ".<br>";
  messageBody += "<b>Current Balance:</b> " + newBalance + ".<br>";
  messageBody += "<b>Initial Balance:</b> " + currentBalance + ".<br>";
  messageBody += "<b>Last Updated Vehicle Cost:</b> " + enteredAmount + ".<br>";

  String Driver_Name = "Adekeye Damilare Lekan";
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
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
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
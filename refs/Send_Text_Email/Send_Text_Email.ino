// #include <Arduino.h>
// #include <WiFi.h>

// #include <ESP_Mail_Client.h>

// #define WIFI_SSID "Network_Connect"
// #define WIFI_PASSWORD "vvvvvvvv"

// /** The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com */
// #define SMTP_HOST "smtp.gmail.com"

// /** The smtp port e.g.
//  * 25  or esp_mail_smtp_port_25
//  * 465 or esp_mail_smtp_port_465
//  * 587 or esp_mail_smtp_port_587
//  */
// // #define SMTP_PORT esp_mail_smtp_port_587  // port 465 is not available for Outlook.com
// #define SMTP_PORT esp_mail_smtp_port_465  // port 465 is not available for Outlook.com

// /* The log in credentials */
// #define AUTHOR_EMAIL "rfidprojectokoro@gmail.com"
// #define AUTHOR_PASSWORD "xxxkvdxwrsdvmngu"

// /* Recipient email address */
// #define RECIPIENT_EMAIL "adekeyedamilarelekan@gmail.com"

// /* Declare the global used SMTPSession object for SMTP transport */
// SMTPSession smtp;

// /* Callback function to get the Email sending status */
// void smtpCallback(SMTP_Status status);

// void setup() {
//   Serial.begin(115200);
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");

//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(300);
//   }
//   Serial.println();
//   Serial.print("Connected with IP: ");
//   Serial.println(WiFi.localIP());
//   Serial.println();

//   /*  Set the network reconnection option */
//   MailClient.networkReconnect(true);


//   smtp.debug(1);

//   /* Set the callback function to get the sending results */
//   smtp.callback(smtpCallback);

//   /* Declare the Session_Config for user defined session credentials */
//   Session_Config config;

//   /* Set the session config */
//   config.server.host_name = SMTP_HOST;
//   config.server.port = SMTP_PORT;
//   config.login.email = AUTHOR_EMAIL;
//   config.login.password = AUTHOR_PASSWORD;

//   /** Assign your host name or you public IPv4 or IPv6 only
//    * as this is the part of EHLO/HELO command to identify the client system
//    * to prevent connection rejection.
//    * If host name or public IP is not available, ignore this or
//    * use loopback address "127.0.0.1".
//    *
//    * Assign any text to this option may cause the connection rejection.
//    */
//   config.login.user_domain = F("127.0.0.1");

//   /** If non-secure port is prefered (not allow SSL and TLS connection), use
//    *  config.secure.mode = esp_mail_secure_mode_nonsecure;
//    *
//    *  If SSL and TLS are always required, use
//    *  config.secure.mode = esp_mail_secure_mode_ssl_tls;
//    *
//    *  To disable SSL permanently (use less program space), define ESP_MAIL_DISABLE_SSL in ESP_Mail_FS.h
//    *  or Custom_ESP_Mail_FS.h
//    */
//   // config.secure.mode = esp_mail_secure_mode_nonsecure;

//   /*
//   Set the NTP config time
//   For times east of the Prime Meridian use 0-12
//   For times west of the Prime Meridian add 12 to the offset.
//   Ex. American/Denver GMT would be -6. 6 + 12 = 18
//   See https://en.wikipedia.org/wiki/Time_zone for a list of the GMT/UTC timezone offsets
//   */
//   // config.time.ntp_server = F("pool.ntp.org,time.nist.gov");  //i commented all these codes.
//   // config.time.gmt_offset = 3;                                //i commented all these codes.
//   // config.time.day_light_offset = 0;                          //i commented all these codes.

//   config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
//   config.time.gmt_offset = 1;        // Nigeria is GMT+1
//   config.time.day_light_offset = 0;  // No daylight saving in Nigeria

//   /* The full message sending logs can now save to file */
//   /* Since v3.0.4, the sent logs stored in smtp.sendingResult will store only the latest message logs */
//   // config.sentLogs.filename = "/path/to/log/file";
//   // config.sentLogs.storage_type = esp_mail_file_storage_type_flash;

//   /** In ESP32, timezone environment will not keep after wake up boot from sleep.
//    * The local time will equal to GMT time.
//    *
//    * To sync or set time with NTP server with the valid local time after wake up boot,
//    * set both gmt and day light offsets to 0 and assign the timezone environment string e.g.

//      config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
//      config.time.gmt_offset = 0;
//      config.time.day_light_offset = 0;
//      config.time.timezone_env_string = "JST-9"; // for Tokyo

//    * The library will get (sync) the time from NTP server without GMT time offset adjustment
//    * and set the timezone environment variable later.
//    *
//    * This timezone environment string will be stored to flash or SD file named "/tze.txt"
//    * which set via config.time.timezone_file.
//    *
//    * See the timezone environment string list from
//    * https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
//    *
//    */

//   /* Declare the message class */
//   SMTP_Message message;

//   /* Set the message headers */
//   message.sender.name = F("Me (adekeyedamilarelekan)");
//   message.sender.email = AUTHOR_EMAIL;
//   String subject = "Sending Payment status for shuttle in CU";
//   message.subject = subject;
//   message.addRecipient(F("Receiver"), RECIPIENT_EMAIL);
//   String textMsg = "This is simple plain text message which contains Chinese and Japanese words.\n";
//   textMsg += "This is the body of the email i want to send as test.\n";
//   textMsg += "I hope that it works.\n";
//   message.text.content = textMsg;

//   /** The content transfer encoding e.g.
//    * enc_7bit or "7bit" (not encoded)
//    * enc_qp or "quoted-printable" (encoded)
//    * enc_base64 or "base64" (encoded)
//    * enc_binary or "binary" (not encoded)
//    * enc_8bit or "8bit" (not encoded)
//    * The default value is "7bit"
//    */

//   message.text.transfer_encoding = "base64";  // recommend for non-ASCII words in message.
//   message.text.charSet = F("utf-8");          // recommend for non-ASCII words in message.

//   /** The message priority
//    * esp_mail_smtp_priority_high or 1
//    * esp_mail_smtp_priority_normal or 3
//    * esp_mail_smtp_priority_low or 5
//    * The default value is esp_mail_smtp_priority_low
//    */
//   message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;

//   // message.response.reply_to = "someone@somemail.com";
//   // message.response.return_path = "someone@somemail.com";

//   /** The Delivery Status Notifications e.g.
//    * esp_mail_smtp_notify_never
//    * esp_mail_smtp_notify_success
//    * esp_mail_smtp_notify_failure
//    * esp_mail_smtp_notify_delay
//    * The default value is esp_mail_smtp_notify_never
//    */
//   // message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

//   // /* Set the custom message header */
//   // message.addHeader(F("Message-ID: <abcde.fghij@gmail.com>"));

//   // For Root CA certificate verification (ESP8266 and ESP32 only)
//   // config.certificate.cert_data = rootCACert;
//   // or
//   // config.certificate.cert_file = "/path/to/der/file";
//   // config.certificate.cert_file_storage_type = esp_mail_file_storage_type_flash; // esp_mail_file_storage_type_sd
//   // config.certificate.verify = true;

//   // The WiFiNINA firmware the Root CA certification can be added via the option in Firmware update tool in Arduino IDE

//   /* Connect to server with the session config */

//   // Library will be trying to sync the time with NTP server if time is never sync or set.
//   // This is 10 seconds blocking process.
//   // If time reading was timed out, the error "NTP server time reading timed out" will show via debug and callback function.
//   // You can manually sync time by yourself with NTP library or calling configTime in ESP32 and ESP8266.
//   // Time can be set manually with provided timestamp to function smtp.setSystemTime.

//   /* Set the TCP response read timeout in seconds */
//   // smtp.setTCPTimeout(10);

//   /* Connect to the server */
//   if (!smtp.connect(&config)) {
//     MailClient.printf("Connection error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
//     return;
//   }

//   /** Or connect without log in and log in later

//      if (!smtp.connect(&config, false))
//        return;

//      if (!smtp.loginWithPassword(AUTHOR_EMAIL, AUTHOR_PASSWORD))
//        return;
//   */

//   if (!smtp.isLoggedIn()) {
//     Serial.println("Not yet logged in.");
//   } else {
//     if (smtp.isAuthenticated())
//       Serial.println("Successfully logged in.");
//     else
//       Serial.println("Connected with no Auth.");
//   }

//   /* Start sending Email and close the session */
//   if (!MailClient.sendMail(&smtp, &message))
//     MailClient.printf("Error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());

//   // to clear sending result log
//   // smtp.sendingResult.clear();
// }

// void loop() {
// }

// /* Callback function to get the Email sending status */
// void smtpCallback(SMTP_Status status) {
//   /* Print the current status */
//   Serial.println(status.info());

//   /* Print the sending result */
//   if (status.success()) {
//     // MailClient.printf used in the examples is for format printing via debug Serial port
//     // that works for all supported Arduino platform SDKs e.g. SAMD, ESP32 and ESP8266.
//     // In ESP8266 and ESP32, you can use Serial.printf directly.

//     Serial.println("----------------");
//     MailClient.printf("Message sent success: %d\n", status.completedCount());
//     MailClient.printf("Message sent failed: %d\n", status.failedCount());
//     Serial.println("----------------\n");

//     for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
//       /* Get the result item */
//       SMTP_Result result = smtp.sendingResult.getItem(i);

//       // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
//       // your device time was synched with NTP server.
//       // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
//       // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)

//       MailClient.printf("Message No: %d\n", i + 1);
//       MailClient.printf("Status: %s\n", result.completed ? "success" : "failed");
//       MailClient.printf("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
//       MailClient.printf("Recipient: %s\n", result.recipients.c_str());
//       MailClient.printf("Subject: %s\n", result.subject.c_str());
//     }
//     Serial.println("----------------\n");

//     // You need to clear sending result as the memory usage will grow up.
//     smtp.sendingResult.clear();
//   }
// }

//BELOW CODE IS RECOMMENDED TO BE USED.
// using it in the void loop.
#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "Network_Connect"
#define WIFI_PASSWORD "vvvvvvvv"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_465

#define AUTHOR_EMAIL "rfidprojectokoro@gmail.com"
#define AUTHOR_PASSWORD "xxxkvdxwrsdvmngu"
// #define RECIPIENT_EMAIL "adekeyedamilarelekan@gmail.com"

SMTPSession smtp;

// Cooldown variables
unsigned long lastSendTime = 0;
const unsigned long COOLDOWN = 60000;  // 1 minute cooldown

String RECIPIENT_EMAIL = "";
void smtpCallback(SMTP_Status status);

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  MailClient.networkReconnect(true);
  smtp.debug(1);
  smtp.callback(smtpCallback);
}

void loop() {
  // // Example condition: Replace with your actual condition
  // bool conditionMet = /* Your condition here */;

  // if (conditionMet && (millis() - lastSendTime >= COOLDOWN)) {
  //   sendEmail();
  //   lastSendTime = millis();
  // }
  if (millis() - lastSendTime >= COOLDOWN) {
    sendEmail();
    lastSendTime = millis();
  }
}

void sendEmail() {
  Session_Config config;
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = F("127.0.0.1");
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 1;
  config.time.day_light_offset = 0;

  SMTP_Message message;
  message.sender.name = F("Me (adekeyedamilarelekan)");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Sending Payment status for shuttle in CU";
  message.addRecipient(F("Receiver"), RECIPIENT_EMAIL);
  message.text.content = "This is simple plain text message...";
  message.text.transfer_encoding = "base64";
  message.text.charSet = F("utf-8");
  message.priority = esp_mail_smtp_priority_low;

  if (!smtp.connect(&config)) {
    Serial.printf("Connection error: %s\n", smtp.errorReason().c_str());
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.printf("Error sending email: %s\n", smtp.errorReason().c_str());
  }

  smtp.closeSession();
  Serial.println("Email sent and session closed.");
}

void smtpCallback(SMTP_Status status) {
  // Existing callback code (unchanged)
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()) {
    // MailClient.printf used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. SAMD, ESP32 and ESP8266.
    // In ESP8266 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    MailClient.printf("Message sent success: %d\n", status.completedCount());
    MailClient.printf("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)

      MailClient.printf("Message No: %d\n", i + 1);
      MailClient.printf("Status: %s\n", result.completed ? "success" : "failed");
      MailClient.printf("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      MailClient.printf("Recipient: %s\n", result.recipients.c_str());
      MailClient.printf("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}
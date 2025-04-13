#define buzzer 27
#define RED_Led 12
#define BLUE_Led 4
#define WiFi_Led 13
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(buzzer, OUTPUT);
  pinMode(RED_Led, OUTPUT);
  pinMode(BLUE_Led, OUTPUT);
  pinMode(WiFi_Led, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(buzzer, HIGH);    // turn the LED on (HIGH is the voltage level)
  digitalWrite(RED_Led, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(BLUE_Led, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(WiFi_Led, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                   // wait for a second
  digitalWrite(buzzer, LOW);     // turn the LED off (LOW is the voltage level)
  digitalWrite(RED_Led, LOW);    // turn the LED off (LOW is the voltage level)
  digitalWrite(BLUE_Led, LOW);   // turn the LED off (LOW is the voltage level)
  digitalWrite(WiFi_Led, LOW);   // turn the LED off (LOW is the voltage level)
  delay(1000);                   // wait for a second
}

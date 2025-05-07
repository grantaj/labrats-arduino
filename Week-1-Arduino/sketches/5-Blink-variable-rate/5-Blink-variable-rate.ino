// Pin definitions
const int ledPin = 12;    // LED connected to digital pin 12
const int potPin = A0;    // Potentiometer connected to analog pin A0

void setup() {
  pinMode(ledPin, OUTPUT);  // Set LED pin as an output
}

void loop() {
  int potValue = analogRead(potPin);          // Read the potentiometer (0 to 1023)
  int delayTime = map(potValue, 0, 1023, 100, 1000); // Map to 100–1000 ms delay

  digitalWrite(ledPin, HIGH);   // Turn the LED on
  delay(delayTime);             // Wait
  digitalWrite(ledPin, LOW);    // Turn the LED off
  delay(delayTime);             // Wait again
}


const int potPin = A0;  // Potentiometer connected to analog pin A0

void setup() {
  Serial.begin(9600);   // Start the Serial communication at 9600 baud
}

void loop() {
  int potValue = analogRead(potPin);  // Read analog value (0 to 1023)
  Serial.println(potValue);           // Print the value to the Serial Monitor
  delay(200);                         // Wait a bit between readings
}

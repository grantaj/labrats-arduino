// Define a variable that tells the arduino
// which pin the LED is connected to
// The on-board LED is at LED_BUILTIN which is
// a macro, euqal to 13 for Uno and Leonardo boards

int ledPin = 13;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(ledPin, HIGH); 
  delay(500);                      
  digitalWrite(ledPin, LOW);
  delay(500);                     
}

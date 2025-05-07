const int ledPin = 12;
const int potPin = A0;
const int buttonPin = 2; // Must be interrupt-capable pin (2 or 3 on Uno)

volatile bool blinking = true;  // Must be volatile because it’s modified in ISR

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Use internal pull-up

  // Attach interrupt to the button pin (falling edge = button press)
  attachInterrupt(digitalPinToInterrupt(buttonPin), toggleBlinking, FALLING);

  Serial.begin(9600);
}

void loop() {
  if (blinking) {
    int potValue = analogRead(potPin);
    int delayTime = map(potValue, 0, 1023, 100, 1000);
    
    digitalWrite(LED_BUILTIN, LOW);

    digitalWrite(ledPin, HIGH);
    delay(delayTime);
    digitalWrite(ledPin, LOW);
    delay(delayTime);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void toggleBlinking() {
  blinking = !blinking;
}


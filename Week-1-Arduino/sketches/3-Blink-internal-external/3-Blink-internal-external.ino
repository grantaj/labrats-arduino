/*
  Blink the internal LED and and an external LED at different rates
*/

const int led1 = 13;      // Built-in LED
const int led2 = 12;      // External LED

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;

const unsigned long interval1 = 1000; // 1 second
const unsigned long interval2 = 100;  // 0.1 seconds

bool led1State = false;
bool led2State = false;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // Blink LED1
  if (currentMillis - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis;
    led1State = !led1State;
    digitalWrite(led1, led1State);
  }

  // Blink LED2
  if (currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    led2State = !led2State;
    digitalWrite(led2, led2State);
  }
}

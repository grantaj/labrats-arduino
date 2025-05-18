#include <USB-MIDI.h>
#include <Bounce2.h>

// Create the default USB MIDI interface (named "MIDI")
USBMIDI_CREATE_DEFAULT_INSTANCE();

const byte BUTTON_PIN = 2;
const byte NOTE = 60;       // Middle C
const byte VELOCITY = 100;
const byte CHANNEL = 1;

Bounce button = Bounce();

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Button connected to GND when pressed
  button.attach(BUTTON_PIN);
  button.interval(5);                 // Debounce interval in ms

  delay(1000);
  MIDI.begin();  // Start USB MIDI

}

void loop() {
  button.update();

  if (button.fell()) {
    // Button just pressed → send Note On
    MIDI.sendNoteOn(NOTE, VELOCITY, CHANNEL);
  }

  if (button.rose()) {
    // Button just released → send Note Off
    MIDI.sendNoteOff(NOTE, 0, CHANNEL);
  }
}


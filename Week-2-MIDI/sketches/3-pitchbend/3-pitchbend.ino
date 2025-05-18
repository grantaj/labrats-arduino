#include <USB-MIDI.h>
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

const byte BUTTON_PIN = 2;
const byte NOTE = 60;
const byte VELOCITY = 100;
const byte CHANNEL = 1;
const byte POT_PIN = A0;

Bounce button = Bounce();
ResponsiveAnalogRead pot(POT_PIN, true);  // use sleep mode = true

bool noteIsOn = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button.attach(BUTTON_PIN);
  button.interval(5);

  delay(1000);  // Wait for USB MIDI host to stabilize
  MIDI.begin();

  // Clear any stuck notes
  for (int n = 0; n < 128; n++) {
    MIDI.sendNoteOff(n, 0, CHANNEL);
  }
}

void loop() {
  button.update();
  pot.update();  // Filtered analog read

  // Handle note on/off from button
  if (button.fell()) {
    MIDI.sendNoteOn(NOTE, VELOCITY, CHANNEL);
    noteIsOn = true;
  }

  if (button.rose()) {
    MIDI.sendNoteOff(NOTE, 0, CHANNEL);
    noteIsOn = false;
  }

  // Only send pitch bend when the value changes
  if (pot.hasChanged()) {
    int analogValue = pot.getValue();  // 0–1023
    int bend = map(analogValue, 0, 1023, -8192, 8191);
    MIDI.sendPitchBend(bend, CHANNEL);

  }
}

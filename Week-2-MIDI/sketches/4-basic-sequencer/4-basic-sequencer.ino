#include <USB-MIDI.h>
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

const byte BUTTON_PIN = 2;
const byte POT_PIN = A0;

const byte VELOCITY = 100;
const byte CHANNEL = 1;

const int LOW_BPM = 60;
const int HIGH_BPM = 180;

Bounce button = Bounce();
ResponsiveAnalogRead tempoPot(POT_PIN, true);  // Filtered analog read

// Cool 8-step sequence (0 = rest)
const byte noteSequence[8] = {36, 0, 38, 43, 36, 41, 38, 0};
const byte SEQ_LENGTH = sizeof(noteSequence) / sizeof(noteSequence[0]);

bool playing = false;
byte stepIndex = 0;
unsigned long lastStepTime = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button.attach(BUTTON_PIN);
  button.interval(5);

  MIDI.begin();
  delay(2000);  // Allow host to stabilize

  for (int i = 0; i < 128; i++) {
    MIDI.sendNoteOff(i, 0, CHANNEL);
  }
}

void loop() {
  button.update();
  tempoPot.update();

  // Toggle start/stop on button press
  if (button.fell()) {
    playing = !playing;

    // Send All Notes Off on stop
    if (!playing) {
      for (int i = 0; i < 128; i++) {
        MIDI.sendNoteOff(i, 0, CHANNEL);
      }
    }

    stepIndex = 0;
    lastStepTime = millis();
  }

  if (!playing) return;

  // Calculate step interval from potentiometer (mapped to BPM)
  int bpm = map(tempoPot.getValue(), 0, 1023, LOW_BPM, HIGH_BPM);
  int stepInterval = 60000 / (bpm * 2);  // 2 steps per beat (16th notes)

  // Time for next step?
  unsigned long now = millis();
  if (now - lastStepTime >= stepInterval) {
    lastStepTime = now;

    byte note = noteSequence[stepIndex];
    stepIndex = (stepIndex + 1) % SEQ_LENGTH;

    // Play new note if not rest
    if (note != 0) {
      MIDI.sendNoteOn(note, VELOCITY, CHANNEL);
    }

    // Turn off previous note
    static byte lastNote = 0;
    if (lastNote != 0) {
      MIDI.sendNoteOff(lastNote, 0, CHANNEL);
    }
    lastNote = note;
  }
}

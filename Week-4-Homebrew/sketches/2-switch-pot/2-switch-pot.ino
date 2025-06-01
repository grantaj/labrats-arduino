#include <USB-MIDI.h>
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();  // creates MIDI instance

// ── PINS ─────────────────────────────────────────────────────────────────────
#define SWITCH_PIN    2    // tactile switch input (with pull-up)
#define POT_PIN       A0   // potentiometer analog input

// ── MIDI CHANNEL ASSIGNMENT ───────────────────────────────────────────────────
#define CHANNEL       1    // send all MIDI messages on channel 1

// ── MIDI PARAMETER RANGES ────────────────────────────────────────────────────
#define NOTE_MIN      48   // C3
#define NOTE_MAX      72   // C5
#define VELOCITY      100  // fixed velocity for note-on

// ── GLOBAL OBJECTS ───────────────────────────────────────────────────────────
Bounce switchDebouncer = Bounce();
// Construct potInput with explicit constructor (pin, sleepEnable, snapMultiplier)
ResponsiveAnalogRead potInput(POT_PIN, true, 0.01);

// Track current note state
bool noteHeld = false;
byte currentNote = (NOTE_MIN + NOTE_MAX) / 2;

void setup() {
  // Initialize USB-MIDI
  MIDI.begin();

  // Configure switch pin with internal pull-up and attach to debouncer
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  switchDebouncer.attach(SWITCH_PIN);
  switchDebouncer.interval(10); // debounce interval (ms)
}

void loop() {
  // ----- HANDLE POTENTIOMETER -----
  potInput.update();
  int potValue = potInput.getValue(); // 0–1023

  // Map pot reading to MIDI note range
  byte mappedNote = map(potValue, 0, 1023, NOTE_MIN, NOTE_MAX);

  // If note value changed while switch is held, update held note
  if (mappedNote != currentNote && noteHeld) {
    // Send Note-Off for old note
    MIDI.sendNoteOff(currentNote, 0, CHANNEL);
    // Update to new note and send Note-On
    currentNote = mappedNote;
    MIDI.sendNoteOn(currentNote, VELOCITY, CHANNEL);
  }
  else {
    // If switch not held, just update currentNote for next press
    currentNote = mappedNote;
  }

  // ----- HANDLE SWITCH -----
  switchDebouncer.update();

  // Detect falling edge (pressed)
  if (switchDebouncer.fell()) {
    // Send Note-On at currentNote
    MIDI.sendNoteOn(currentNote, VELOCITY, CHANNEL);
    noteHeld = true;
  }

  // Detect rising edge (released)
  if (switchDebouncer.rose()) {
    // Send Note-Off
    MIDI.sendNoteOff(currentNote, 0, CHANNEL);
    noteHeld = false;
  }
}

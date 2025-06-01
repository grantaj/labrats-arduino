#include <USB-MIDI.h>
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();  // creates MIDI instance

// ── PINS ─────────────────────────────────────────────────────────────────────
#define SWITCH_PIN    2    // tactile switch input (with pull-up)
#define POT_PIN       A0   // potentiometer analog input
#define LDR_PIN       A1   // light-dependent resistor input

// ── MIDI CHANNEL ASSIGNMENT ───────────────────────────────────────────────────
#define CHANNEL       1    // send all MIDI messages on channel 1

// ── COMMON MIDI CONTROL NUMBERS ──────────────────────────────────────────────
#define CC_MODWHEEL   1    // modulation wheel
#define CC_AFTERTOUCH 128  // channel pressure (requires special handling)
#define PITCH_BEND    129  // pitch bend (not a CC; symbolic only)

// ── MIDI PARAMETER RANGES ────────────────────────────────────────────────────
#define NOTE_MIN      48   // C3
#define NOTE_MAX      72   // C5
#define VELOCITY      100  // fixed velocity for note-on
#define LDR_CC        16   // control change number for LDR output

// ── GLOBAL OBJECTS ───────────────────────────────────────────────────────────
Bounce switchDebouncer = Bounce();
// Construct potInput with explicit constructor (pin, sleepEnable, snapMultiplier)
ResponsiveAnalogRead potInput(POT_PIN, true, 0.01);
ResponsiveAnalogRead ldrInput(LDR_PIN, true, 0.01);

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
    MIDI.sendNoteOff(currentNote, 0, CHANNEL);
    currentNote = mappedNote;
    MIDI.sendNoteOn(currentNote, VELOCITY, CHANNEL);
  } else {
    currentNote = mappedNote;
  }

  // ----- HANDLE SWITCH -----
  switchDebouncer.update();

  if (switchDebouncer.fell()) {
    MIDI.sendNoteOn(currentNote, VELOCITY, CHANNEL);
    noteHeld = true;
  }

  if (switchDebouncer.rose()) {
    MIDI.sendNoteOff(currentNote, 0, CHANNEL);
    noteHeld = false;
  }

  // ----- HANDLE LDR AS CC MESSAGE -----
  ldrInput.update();
  if (ldrInput.hasChanged()) {
    int ldrValue = ldrInput.getValue(); // 0–1023
    byte ccValue = map(ldrValue, 0, 1023, 0, 127);
    MIDI.sendControlChange(LDR_CC, ccValue, CHANNEL);
  }
}

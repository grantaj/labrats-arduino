/*
  MIDI Step Sequencer - Beginner-Friendly USB MIDI Controller

  USER INTERFACE OVERVIEW:

  - Button A (Mode & Play Toggle):
    * Short press: Start/Stop playback (only in Play Mode)
    * Long press: Toggle between Note Input Mode and Play Mode

  - Button B (Step Entry):
    * In Note Input Mode:
      - Long press: Preview note from potentiometer and store on release
      - Short press: Insert a rest and advance to next step
    * In Play Mode (when stopped):
      - Short press: Manually advance one step and play that note

  - Potentiometer (A0):
    * In Input Mode: Select pitch for current step
    * In Play Mode: Sets the tempo (BPM)

  - NeoPixel Strip (8 LEDs):
    * Blue: Current step in input mode
    * Red: Current step while playing
    * Dim Red: Current step is a rest
    * Green: Step contains a note
    * Off: Step is a rest and not selected
*/

#include <USB-MIDI.h>
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>
#include <Adafruit_NeoPixel.h>

// -------------------------
// Configuration Constants
// -------------------------

// LED Colors
#define COLOR_NOTE pixels.Color(0, 255, 0)
#define COLOR_PLAYHEAD pixels.Color(0, 0, 255)  // Blue for playback
#define COLOR_INPUT_CURSOR pixels.Color(255, 0, 0) // Red for input step
#define COLOR_DIM_PLAYHEAD pixels.Color(0, 0, 30)  // Dim blue for rest playhead
#define COLOR_OFF 0
// -------------------------

USBMIDI_CREATE_DEFAULT_INSTANCE();

#define BUTTON_A_PIN 2
#define BUTTON_B_PIN 3
#define POT_PIN A0
#define LED_PIN 6
#define NUM_STEPS 8
#define VELOCITY 100
#define CHANNEL 1

const int LOW_NOTE = 36;
const int HIGH_NOTE = 60;
const int LOW_BPM = 60;
const int HIGH_BPM = 180;

// -------------------------
// State Variables
// -------------------------

Bounce buttonA = Bounce();
Bounce buttonB = Bounce();
ResponsiveAnalogRead tempoPot(POT_PIN, true);
Adafruit_NeoPixel pixels(NUM_STEPS, LED_PIN, NEO_GRB + NEO_KHZ800);

byte noteSequence[NUM_STEPS] = {36, 0, 38, 43, 36, 41, 38, 0};
byte currentStep = 0;
byte playHead = 0;
bool playing = false;
bool inInputMode = false;

unsigned long lastStepTime = 0;
unsigned long buttonAPressTime = 0;
bool buttonAHeld = false;
unsigned long noteOnTime = 0;
byte lastNote = 0;
byte previewNote = 0;
bool previewing = false;

// -------------------------
// Setup
// -------------------------

void setup() {
  MIDI.begin();
  delay(2000);
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  buttonA.attach(BUTTON_A_PIN);
  buttonB.attach(BUTTON_B_PIN);
  buttonA.interval(5);
  buttonB.interval(5);
  pixels.begin();
  pixels.show();
  clearAllNotes();
  previewNote = map(analogRead(POT_PIN), 0, 1023, LOW_NOTE, HIGH_NOTE);
}

// -------------------------
// Main Loop
// -------------------------

void loop() {
  buttonA.update();
  buttonB.update();
  tempoPot.update();

  int stepInterval = getStepInterval();

  handleButtonA();
  if (inInputMode) handleInputMode();
  else handlePlayMode(stepInterval);

  if (playing && millis() - lastStepTime >= stepInterval) {
    lastStepTime = millis();
    advanceStep();
  }

  checkNoteOffTimeout(stepInterval);
  updateLEDs();
}

// -------------------------
// Input Mode Logic
// -------------------------

void handleInputMode() {
  static unsigned long pressStartTime = 0;
  const unsigned long longPressThreshold = 250; // ms

  if (buttonB.fell()) {
    pressStartTime = millis();
    previewing = false;
    previewNote = 0; // reset preview note
  }

  if (buttonB.read() == LOW && (millis() - pressStartTime > longPressThreshold)) {
    byte note = map(analogRead(POT_PIN), 0, 1023, LOW_NOTE, HIGH_NOTE);
    if (!previewing || note != previewNote) {
      if (previewNote > 0) MIDI.sendNoteOff(previewNote, 0, CHANNEL);
      MIDI.sendNoteOn(note, VELOCITY, CHANNEL);
      previewNote = note;
      previewing = true;
    }
  }

  if (buttonB.rose()) {
    if (millis() - pressStartTime > longPressThreshold && previewNote > 0) {
      MIDI.sendNoteOff(previewNote, 0, CHANNEL);
      noteSequence[currentStep] = previewNote;
    } else {
      noteSequence[currentStep] = 0; // short press = rest
    }
    previewNote = 0;
    previewing = false;
    currentStep = (currentStep + 1) % NUM_STEPS;
  }
}

// -------------------------
// Play Mode Logic
// -------------------------

void handlePlayMode(int stepInterval) {
  if (!playing && buttonB.fell()) {
    advanceStep();
  }
}

// -------------------------
// Handle Button A (mode/play toggle)
// -------------------------

void handleButtonA() {
  if (buttonA.fell()) {
    buttonAPressTime = millis();
    buttonAHeld = false;
  }
  if (buttonA.read() == LOW && !buttonAHeld && millis() - buttonAPressTime > 500) {
    inInputMode = !inInputMode;
    playing = false;
    currentStep = 0;
    clearAllNotes();
    buttonAHeld = true;
  }
  if (buttonA.rose() && !buttonAHeld) {
    if (!inInputMode) {
      playing = !playing;
      if (!playing) clearAllNotes();
      playHead = 0;
    }
  }
}

// -------------------------
// Advance Step
// -------------------------

void advanceStep() {
  if (lastNote > 0) {
    MIDI.sendNoteOff(lastNote, 0, CHANNEL);
    lastNote = 0;
  }
  playHead = (playHead + 1) % NUM_STEPS;
  byte currentNote = noteSequence[playHead];
  if (currentNote > 0) {
    MIDI.sendNoteOn(currentNote, VELOCITY, CHANNEL);
    lastNote = currentNote;
    noteOnTime = millis();
  }
}

// -------------------------
// Step Timing Based on Pot
// -------------------------

int getStepInterval() {
  int bpm = map(analogRead(POT_PIN), 0, 1023, LOW_BPM, HIGH_BPM);
  return 60000 / (bpm * 2);
}

// -------------------------
// LED Feedback
// -------------------------

void updateLEDs() {
  for (int i = 0; i < NUM_STEPS; i++) {
    if (!inInputMode && i == playHead) {
      pixels.setPixelColor(i, noteSequence[i] != 0 ? COLOR_PLAYHEAD : COLOR_DIM_PLAYHEAD);
    } else if (inInputMode && i == currentStep) {
      pixels.setPixelColor(i, COLOR_INPUT_CURSOR);
    } else if (noteSequence[i] != 0) {
      pixels.setPixelColor(i, COLOR_NOTE);
    } else {
      pixels.setPixelColor(i, COLOR_OFF);
    }
  }
  pixels.show();
}

// -------------------------
// MIDI Cleanup
// -------------------------

void clearAllNotes() {
  for (int i = 0; i < 128; i++) {
    MIDI.sendNoteOff(i, 0, CHANNEL);
  }
}

// -------------------------
// Auto Note-Off Timeout
// -------------------------

void checkNoteOffTimeout(int stepInterval) {
  if (!playing && !inInputMode && lastNote > 0) {
    int holdTime = stepInterval * 0.8;
    if (millis() - noteOnTime > holdTime) {
      MIDI.sendNoteOff(lastNote, 0, CHANNEL);
      lastNote = 0;
    }
  }
}

#include <USB-MIDI.h>
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();  // creates MIDI instance

// ── PINS ─────────────────────────────────────────────────────────────────────
#define SWITCH_PIN    4    // tactile switch input (with pull-up)
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
#define ACCEL_X_CC    17
#define ACCEL_Y_CC    18
#define ACCEL_Z_CC    19

// ── GLOBAL OBJECTS ───────────────────────────────────────────────────────────
Bounce switchDebouncer = Bounce();
ResponsiveAnalogRead potInput(POT_PIN, true, 0.01);
ResponsiveAnalogRead ldrInput(LDR_PIN, true, 0.01);
Adafruit_MPU6050 mpu;

bool noteHeld = false;
byte currentNote = (NOTE_MIN + NOTE_MAX) / 2;

void setup() {
  MIDI.begin();

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  switchDebouncer.attach(SWITCH_PIN);
  switchDebouncer.interval(10);

  Wire.begin();
  if (!mpu.begin()) {
    // MPU6050 not found
    while (1);
  }
}

void loop() {
  // ----- HANDLE POTENTIOMETER -----
  potInput.update();
  int potValue = potInput.getValue();
  byte mappedNote = map(potValue, 0, 1023, NOTE_MIN, NOTE_MAX);

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
    int ldrValue = ldrInput.getValue();
    byte ccValue = map(ldrValue, 0, 1023, 0, 127);
    MIDI.sendControlChange(LDR_CC, ccValue, CHANNEL);
  }

  // ----- HANDLE ACCELEROMETER AS CC MESSAGES -----
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  byte xVal = map((int)(a.acceleration.x * 1000), -16000, 16000, 0, 127);
  byte yVal = map((int)(a.acceleration.y * 1000), -16000, 16000, 0, 127);
  byte zVal = map((int)(a.acceleration.z * 1000), -16000, 16000, 0, 127);

  MIDI.sendControlChange(ACCEL_X_CC, xVal, CHANNEL);
  MIDI.sendControlChange(ACCEL_Y_CC, yVal, CHANNEL);
  MIDI.sendControlChange(ACCEL_Z_CC, zVal, CHANNEL);
}

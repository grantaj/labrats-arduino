#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

// ── MIDI CHANNEL ASSIGNMENTS (1–16) ─────────────────────────────────────────
#define CHANNEL    1   // send notes on MIDI channel 1

// ── DEFAULT NOTE PARAMETERS ─────────────────────────────────────────────────
#define NOTE_NUMBER     60  // middle C
#define NOTE_VELOCITY   100 // (0–127)

// ── TIMING ───────────────────────────────────────────────────────────────────
#define NOTE_DURATION   500 // ms on, then 500 ms off

void setup() {
  // No pin- or serial setup needed for this simple USB-MIDI demo
  MIDI.begin(); 
}

void loop() {
  // 1) Send Note-On
  MIDI.sendNoteOn(NOTE_NUMBER, NOTE_VELOCITY, CHANNEL);
  delay(NOTE_DURATION);

  // 2) Send Note-Off
  MIDI.sendNoteOff(NOTE_NUMBER, 0, CHANNEL);
  delay(NOTE_DURATION);
}

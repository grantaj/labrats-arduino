#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// --- Constants ---
const byte MIDI_CHANNEL = 2;
const byte VELOCITY = 110;
const int STEP_DURATION = 94;  // ~160 BPM

// --- Sequences (16 steps each) ---
const byte sequence[] = {
   36, 0, 38, 41,
  43, 38, 0, 36,
  36, 0, 43, 0,
  41, 38, 36, 0
};

const int SEQ_LEN = sizeof(sequence) / sizeof(sequence[0]);


void allNotesOff(byte channel) {
  MIDI.sendControlChange(123, 0, channel);  // All Notes Off
  MIDI.sendControlChange(120, 0, channel);  // All Sound Off
  MIDI.sendControlChange(121, 0, channel);  // Reset Controllers
}

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Reset both channels on startup
  allNotesOff(MIDI_CHANNEL);
}

void loop() {
  for (int i = 0; i < SEQ_LEN; i++) {
    byte note = sequence[i];


    // Send VCO1+2 note (if non-zero)
    if (note != 0) {
      MIDI.sendNoteOn(note, VELOCITY, MIDI_CHANNEL);
    }

    delay(STEP_DURATION);

    // Turn off VCO1+2 note
    if (note != 0) {
     MIDI.sendNoteOff(note, 0, MIDI_CHANNEL);
    }


    delay(1);  // groove gap
  }
}



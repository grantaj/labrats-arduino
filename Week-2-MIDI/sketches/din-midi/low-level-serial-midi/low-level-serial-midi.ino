// Basic MIDI Note On/Off demo using 5-pin DIN via Serial TX
// Baud rate: 31250 (MIDI standard)

const byte MIDI_CHANNEL = 1;   // MIDI channels are 0–15 (0 = Channel 1)
const byte NOTE = 60;          // Middle C
const byte VELOCITY = 100;     // How hard the note is played
const int NOTE_DURATION = 500; // milliseconds

void setup() {
  Serial.begin(31250);  // Start MIDI serial at standard baud rate
}

void loop() {
  // Send Note On: 0x90 + channel
  Serial.write(0x90 | MIDI_CHANNEL);  // Note On message
  Serial.write(NOTE);                 // Note number
  Serial.write(VELOCITY);            // Velocity

  delay(NOTE_DURATION);              // Hold note

  // Send Note Off: either 0x80 or Note On with velocity 0
  Serial.write(0x90 | MIDI_CHANNEL);  // Note On with velocity 0 (alternative Note Off)
  Serial.write(NOTE);
  Serial.write(0);                    // Velocity 0 = Note Off

  delay(1000);  // Wait before repeating
}

// Basic MIDI Note On + Pitch Bend demo using Serial.write()

const byte MIDI_CHANNEL = 1;       // MIDI channel 1 (0–15)
const byte NOTE = 60;              // Middle C
const byte VELOCITY = 100;
const int STEP_DURATION = 30;      // Time between pitch bend steps
const int NOTE_DURATION = 2000;    // Total note time in ms

void setup() {
  Serial.begin(31250);  // MIDI baud rate
}

void loop() {
  // Send Note On (status, note, velocity)
  Serial.write(0x90 | MIDI_CHANNEL);  // 0x90 = Note On
  Serial.write(NOTE);
  Serial.write(VELOCITY);

  // Sweep Pitch Bend from center (8192) to max (16383)
  for (int value = 8192; value <= 16383; value += 256) {
    byte lsb = value & 0x7F;           // lower 7 bits
    byte msb = (value >> 7) & 0x7F;    // upper 7 bits

    Serial.write(0xE0 | MIDI_CHANNEL); // Pitch Bend message
    Serial.write(lsb);
    Serial.write(msb);

    delay(STEP_DURATION);
  }

  // Send Note Off (same status as Note On, velocity 0)
  Serial.write(0x90 | MIDI_CHANNEL);
  Serial.write(NOTE);
  Serial.write(0);

  delay(100);  // Wait before repeating
}


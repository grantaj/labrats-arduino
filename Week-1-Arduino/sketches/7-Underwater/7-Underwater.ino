#include <Adafruit_NeoPixel.h>
#include <math.h>

#define PIN        6      // NeoPixel data pin
#define NUMPIXELS  8      // Number of LEDs

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

float timeOffset = 0;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  timeOffset += 0.05;  // Controls speed of movement

  for (int i = 0; i < NUMPIXELS; i++) {
    float wave = sin(timeOffset + i * 0.6);  // Offset each pixel in time
    float brightness = (wave + 1.0) * 0.5;   // Normalize to 0–1
    int r = 0;
    int g = brightness * 30;   // Very soft green
    int b = brightness * 80 + 20; // Deep blue with a floor

    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  strip.show();
  delay(30);
}

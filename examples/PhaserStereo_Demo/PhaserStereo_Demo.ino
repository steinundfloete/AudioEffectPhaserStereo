#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "AudioEffectPhaserStereo.h"

// Audio connections
AudioInputI2S         i2sIn;
AudioEffectPhaserStereo phaser2;
AudioOutputI2S        i2sOut;

AudioConnection c1(i2sIn, 0, phaser2, 0);
AudioConnection c2(i2sIn, 1, phaser2, 1);
AudioConnection c3(phaser2, 0, i2sOut, 0);
AudioConnection c4(phaser2, 1, i2sOut, 1);

void setup() {
  AudioMemory(30);

  Serial.begin(115200);
  Serial.println("AudioEffectPhaserStereo Demo");

  phaser2.setStages(4);           // 4-stage = Small Stone style
  phaser2.setRate(0.4f);          // LFO speed in Hz
  phaser2.setDepth(1.0f);         // full modulation depth
  phaser2.setFeedback(0.6f);      // resonance
  phaser2.setMix(0.8f);           // mostly wet
  phaser2.setLfoPhaseOffset(0.5f);// 180° stereo
  phaser2.setNegativeResonance(false);

  Serial.println("Phaser ready!");
}

void loop() {
  // Nothing to do here – runs in audio interrupt
}

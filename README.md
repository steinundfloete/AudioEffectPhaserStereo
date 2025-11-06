# AudioEffectPhaserStereo

A stereo analog-style **phaser effect** for the [Teensy Audio Library](https://www.pjrc.com/teensy/td_libs_Audio.html).

Implements a cascaded all-pass filter network with shared modulation, closely mimicking the sound of analog phasers such as the **Electro-Harmonix Small Stone** or **MXR Phase 90**.  
Supports stereo modulation with adjustable phase offset and optional negative resonance for a hollow, vocal tone.

---

## ✨ Features

- ✅ Standard Teensy **AudioStream (int16)** compatibility  
- ✅ 1–12 all-pass stages per channel  
- ✅ Adjustable **rate**, **depth**, **feedback**, **mix**  
- ✅ Stereo **LFO phase offset** (0–1 normalized)  
- ✅ **Negative resonance** switch (invert feedback)  


---

## 🧠 How it works

Each audio channel passes through a cascade of first-order all-pass filters.  
All stages share one modulated coefficient `a`, controlled by a sine-wave LFO.  
The cumulative phase shift produces multiple moving notches when mixed with the dry signal — the hallmark of classic phasers.

Unlike multi-band digital phasers, this design mimics the *analog topology* of vintage pedals:  
shared modulation → warm, cohesive movement and natural “whoosh”.

---

## 🔧 Usage Example

```cpp
#include <Audio.h>
#include "AudioEffectPhaserStereo.h"

AudioInputI2S         i2sIn;
AudioEffectPhaserStereo phaser2;
AudioOutputI2S        i2sOut;

AudioConnection c1(i2sIn, 0, phaser2, 0);
AudioConnection c2(i2sIn, 1, phaser2, 1);
AudioConnection c3(phaser2, 0, i2sOut, 0);
AudioConnection c4(phaser2, 1, i2sOut, 1);

void setup() {
  AudioMemory(30);

  phaser2.setStages(4);
  phaser2.setRate(0.4f);
  phaser2.setDepth(1.0f);
  phaser2.setFeedback(0.6f);
  phaser2.setMix(0.8f);
  phaser2.setLfoPhaseOffset(0.5f); // 180° stereo width
  phaser2.setNegativeResonance(false);
}

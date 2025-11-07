#ifndef AudioEffectPhaserStereo_h_
#define AudioEffectPhaserStereo_h_

#include <Arduino.h>
#include "AudioStream.h"

/**
 * Copyright (c) 2025 by Uli Schmidt steinundfloete@online.de
 * V1.0 11/06/2025 
 * 
 * @brief Stereo phaser effect for the Teensy Audio Library (16-bit blocks).
 * 
 * Implements a cascaded all-pass filter network modulated by an LFO, 
 * emulating analog-style phasers such as the Electro-Harmonix Small Stone
 * or MXR Phase 90. Each channel has independent feedback and a phase-offset LFO
 * for wide stereo imaging.
 * 
 * Features:
 * - 1–12 all-pass stages per channel
 * - Adjustable rate, depth, feedback, and wet/dry mix
 * - Stereo LFO phase offset (0–1 normalized)
 * - Optional negative resonance (inverted feedback)
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
class AudioEffectPhaserStereo : public AudioStream
{
public:
    AudioEffectPhaserStereo();

    /**
     * @brief Audio processing entry point. Called automatically by the Teensy audio engine.
     */
    virtual void update(void) override;

    /**
     * @brief Sets the LFO modulation rate.
     * @param hz Frequency in Hertz (0.01 – 10.0). Default 0.35 Hz.
     */
    void setRate(float hz);

    /**
     * @brief Sets modulation depth (0.0 – 1.0).
     * Controls the sweep range of the all-pass coefficients.
     */
    void setDepth(float d);

    /**
     * @brief Sets the feedback amount (-0.99 – +0.99).
     * Positive values give classic resonance; negative if negativeResonance is enabled.
     */
    void setFeedback(float fb);

    /**
     * @brief Sets wet/dry mix (0.0 = dry, 1.0 = full effect).
     */
    void setMix(float m);

    /**
     * @brief Sets the number of all-pass stages (1–12).
     * More stages → denser phasing.
     */
    void setStages(uint8_t n);

    /**
     * @brief Sets the min/max range for the all-pass coefficient.
     * @param amin Minimum coefficient (0.0–0.99)
     * @param amax Maximum coefficient (amin–0.99)
     */
    void setCoeffRange(float amin, float amax);

    /**
     * @brief Sets the phase offset between left/right LFOs.
     * @param norm Normalized value 0.0–1.0 corresponding to 0–2π radians.
     * 0 = mono, 0.5 = 180°, 1.0 = full cycle.
     */
    void setLfoPhaseOffset(float norm);

    /**
     * @brief Enables or disables negative resonance.
     * @param enable true = invert feedback polarity, false = normal.
     */
    void setNegativeResonance(bool enable);

    /**
     * @brief Resets internal filter states and LFO phase.
     */
    void reset();

private:
    audio_block_t *inputQueueArray[2];
    static const uint8_t MAX_STAGES = 12;

    float z1[2][MAX_STAGES];
    float fbState[2];

    uint8_t numStages;
    float rate, depth, feedback, mix;
    float aMin, aMax;

    float lfoCos, lfoSin;
    float lfoCosDelta, lfoSinDelta;
    float lfoPhaseOffset;
    bool  negativeResonance;

    void init();
};

#endif // AudioEffectPhaserStereo_h_

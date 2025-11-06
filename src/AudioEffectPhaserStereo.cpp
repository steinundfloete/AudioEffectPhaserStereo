#include "AudioEffectPhaserStereo.h"

AudioEffectPhaserStereo::AudioEffectPhaserStereo() 
    : AudioStream(2, inputQueueArray)
{
    init();
}

void AudioEffectPhaserStereo::init() {
    numStages = 6;
    rate = 0.35f;
    depth = 0.8f;
    feedback = 0.6f;
    mix = 0.7f;
    aMin = 0.2f;
    aMax = 0.8f;
    lfoPhaseOffset = 3.14159265358979f; // 180°
    negativeResonance = false;

    for (uint8_t c = 0; c < 2; c++) {
        fbState[c] = 0.0f;
        for (uint8_t s = 0; s < MAX_STAGES; s++) z1[c][s] = 0.0f;
    }

    lfoCos = 1.0f;
    lfoSin = 0.0f;
    setRate(rate);
}

void AudioEffectPhaserStereo::update(void)
{
    audio_block_t *blockL = receiveWritable(0);
    audio_block_t *blockR = receiveWritable(1);

    if (!blockL && !blockR) return;

    // Handle missing channels by duplicating
    if (!blockL && blockR) {
        blockL = allocate();
        if (!blockL) { release(blockR); return; }
        memcpy(blockL->data, blockR->data, AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
    }
    if (!blockR && blockL) {
        blockR = allocate();
        if (!blockR) { release(blockL); return; }
        memcpy(blockR->data, blockL->data, AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
    }

    const uint8_t stages = numStages;
    const float fb = negativeResonance ? -feedback : feedback;
    const float wet = mix;
    const float dry = 1.0f - wet;
    const float amin = aMin;
    const float amax = aMax;
    const float aRange = (amax - amin) * depth;

    float cosP = lfoCos;
    float sinP = lfoSin;
    const float cd = lfoCosDelta;
    const float sd = lfoSinDelta;
    const float phaseOffCos = cos(lfoPhaseOffset);
    const float phaseOffSin = sin(lfoPhaseOffset);

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // LFO update
        float newCos = cosP * cd - sinP * sd;
        float newSin = sinP * cd + cosP * sd;
        cosP = newCos;
        sinP = newSin;

        float modL = 0.5f * (sinP + 1.0f);
        float sinR = sinP * phaseOffCos + cosP * phaseOffSin;
        float modR = 0.5f * (sinR + 1.0f);

        float aL = amin + aRange * modL;
        float aR = amin + aRange * modR;

        float xL = (float)blockL->data[i] / 32768.0f;
        float xR = (float)blockR->data[i] / 32768.0f;

        float inLfb = xL + fb * fbState[0];
        float inRfb = xR + fb * fbState[1];

        // Left
        float xl = inLfb;
        for (uint8_t s = 0; s < stages; s++) {
            float y = -aL * xl + z1[0][s];
            z1[0][s] = xl + aL * y;
            xl = y;
        }
        fbState[0] = xl;

        // Right
        float xr = inRfb;
        for (uint8_t s = 0; s < stages; s++) {
            float y = -aR * xr + z1[1][s];
            z1[1][s] = xr + aR * y;
            xr = y;
        }
        fbState[1] = xr;

        float outLf = dry * xL + wet * xl;
        float outRf = dry * xR + wet * xr;

        blockL->data[i] = (int16_t)(constrain(outLf, -1.0f, 1.0f) * 32767.0f);
        blockR->data[i] = (int16_t)(constrain(outRf, -1.0f, 1.0f) * 32767.0f);
    }

    lfoCos = cosP;
    lfoSin = sinP;

    transmit(blockL, 0);
    transmit(blockR, 1);
    release(blockL);
    release(blockR);
}

// ---------- Setters ----------

void AudioEffectPhaserStereo::setRate(float hz) {
    hz = constrain(hz, 0.01f, 10.0f);
    rate = hz;
    const float fs = AUDIO_SAMPLE_RATE_EXACT;
    float dphi = 2.0f * 3.14159265358979f * rate / fs;
    lfoCosDelta = cosf(dphi);
    lfoSinDelta = sinf(dphi);
}

void AudioEffectPhaserStereo::setDepth(float d) {
    depth = constrain(d, 0.0f, 1.0f);
}

void AudioEffectPhaserStereo::setFeedback(float fb) {
    feedback = constrain(fb, -0.99f, 0.99f);
}

void AudioEffectPhaserStereo::setMix(float m) {
    mix = constrain(m, 0.0f, 1.0f);
}

void AudioEffectPhaserStereo::setStages(uint8_t n) {
    if (n < 1) n = 1;
    if (n > MAX_STAGES) n = MAX_STAGES;
    numStages = n;
}

void AudioEffectPhaserStereo::setCoeffRange(float amin_, float amax_) {
    amin_ = constrain(amin_, 0.0f, 0.99f);
    amax_ = constrain(amax_, amin_ + 0.01f, 0.99f);
    aMin = amin_;
    aMax = amax_;
}

void AudioEffectPhaserStereo::setLfoPhaseOffset(float norm) {
    norm = constrain(norm, 0.0f, 1.0f);
    lfoPhaseOffset = norm * 2.0f * 3.14159265358979f;
}

void AudioEffectPhaserStereo::setNegativeResonance(bool enable) {
    negativeResonance = enable;
}

void AudioEffectPhaserStereo::reset() {
    init();
}

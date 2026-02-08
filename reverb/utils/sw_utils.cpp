#include "sw_utils.h"

void LFO::initLFO(float frequency)
{
    amplitude_ = 1.0f;
    frequency_ = frequency;
    phase_ = -1.0f;
}

void LFO::setAmplitude(float new_amplitude)
{
    amplitude_ = new_amplitude;
}

void LFO::setSampleRate(float sample_rate)
{
    sample_rate_ = sample_rate;
}

float LFO::process()
{
    float value = amplitude_ * sinf(2.0f * M_PI * phase_);
    phase_ += frequency_ / sample_rate_;
    if (phase_ >= 1.0f)
    {
        phase_ = -1.0f;
    }
    return value;
}

float checkGain(float gain)
{
    if (gain <= 0.0f)
    {
        gain = 0.0f;
    }
    else if (gain >= 1.0f)
    {
        gain = 1.0f;
    }
    return gain;
}

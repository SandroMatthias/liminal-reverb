#include "lp_filter.h"

float LowPassFilter_OnePole::g_;
float LowPassFilter_OnePole::sample_rate_;

void LowPassFilter_OnePole::init(float sample_rate)
{
    sample_rate_ = sample_rate;
    lp_y_ = 0.0f;
}

void LowPassFilter_OnePole::setCutoff(float freq_cutoff)
{
    float decay = expf(-2.0f * M_PI * freq_cutoff / sample_rate_);
    g_ = 1.0f - decay;
}

float LowPassFilter_OnePole::processLowPass(float input)
{
    lp_y_ += g_ * (input - lp_y_); 
    return lp_y_;
}
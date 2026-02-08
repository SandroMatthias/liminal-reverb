#include "hp_filter.h"

float HighPassFilter_OnePole::g_;
float HighPassFilter_OnePole::sample_rate_;

void HighPassFilter_OnePole::init(float sample_rate)
{
    sample_rate_ = sample_rate;
    hp_y_ = 0.0f;
    hp_input_prev_ = 0.0f;
}

void HighPassFilter_OnePole::setCutoff(float freq_cutoff)
{
    g_ = expf(-2.0f * M_PI * freq_cutoff / sample_rate_);
}

float HighPassFilter_OnePole::processHighPass(float input)
{
    hp_y_ = g_ * (hp_y_ + input - hp_input_prev_);
    hp_input_prev_ = input;
    return hp_y_;
}

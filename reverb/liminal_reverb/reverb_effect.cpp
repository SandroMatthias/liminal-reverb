#include "reverb_effect.h"

void Reverb::initReverb(unsigned int (&hadamard_lengths)[DELAY_LINE_NUM],
                        unsigned int (&householder_lengths)[DELAY_LINE_NUM],
                        unsigned int (&ap_lengths)[DELAY_LINE_NUM_AP],
                        unsigned int (&ap_long_lengths)[DELAY_LINE_NUM_AP],
                        float lfo_frequency,
                        float sample_rate)
{
    hpf_o_.init(sample_rate);
    lpf_o_.init(sample_rate);
    lfo_.setSampleRate(sample_rate);
    apf_s_.init(ap_lengths);
    apf_s_long_.init(ap_long_lengths);
    fdn_.initHadamard(hadamard_lengths, sample_rate);
    fdn_.initHouseholder(householder_lengths, sample_rate);
    lfo_.initLFO(lfo_frequency);
    apf_gain_ = 0.5f;
    apf_gain_long_ = 0.4f;

    for (size_t n = 0; n < DELAY_LINE_NUM; n++)
    {
        hadamard_lengths_[n] = hadamard_lengths[n];
    }
}

void Reverb::setParameters(bool bypass_freeze, float feedback_pot_CV, float modulation_pot_CV, float lowpass_pot_CV, float highpass_pot_CV)
{
    bypass_freeze_ = bypass_freeze;
    feedback_pot_CV_ = feedback_pot_CV;
    modulation_pot_CV_ = modulation_pot_CV;
    lpf_o_.setCutoff(lowpass_pot_CV);
    hpf_o_.setCutoff(highpass_pot_CV);
}

float Reverb::processReverb(float input)
{
    float diffused_sample = processDiffusionStage(input);
    float reverbated_sample = processFDNStage(diffused_sample);
    return reverbated_sample;
}

float Reverb::processDiffusionStage(float input)
{
    float apf = apf_s_.processAllpass(input, apf_gain_);
    float apf_long = apf_s_long_.processAllpass(apf, apf_gain_long_);
    apf_mixed_ = 0.5f * apf + 0.5f * apf_long;
    float output = lpf_o_.processLowPass(apf_mixed_);
    return output;
}

float Reverb::processFDNStage(float input)
{
    float hadamard = fdn_.processFDNHadamard(input, feedback_pot_CV_, bypass_freeze_);
    float householder;
    if (bypass_freeze_)
    {
        householder = fdn_.processFDNHousholder(hadamard, feedback_pot_CV_) + 0.5f * input;
    }
    else
    {
        householder = fdn_.processFDNHousholder(hadamard, feedback_pot_CV_) + 0.25f * apf_mixed_;
    }
    float output = hpf_o_.processHighPass(householder);

    return output;
}

void Reverb::modulationStage()
{
    lfo_.setAmplitude(modulation_pot_CV_);
    float modulation = lfo_.process();
    for (size_t n = 0; n < DELAY_LINE_NUM; n++)
    {
        float delay_length = hadamard_lengths_[n] + modulation;
        fdn_.hadamard_delay_lines[n].SetDelay((float)delay_length);
    }
}
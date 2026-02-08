#ifndef REVERB__EFFECT_H
#define REVERB__EFFECT_H

#include "daisysp.h"
#include "dsplib.h"

class Reverb
{
    public:
        Reverb(){}
        ~Reverb(){}

        void initReverb(unsigned int(&hadamard_lengths)[DELAY_LINE_NUM], 
                        unsigned int(&householder_lengths)[DELAY_LINE_NUM], 
                        unsigned int (&ap_lengths)[DELAY_LINE_NUM_AP], unsigned int (&ap_long_lengths)[DELAY_LINE_NUM_AP],
                        float lfo_frequency,
                        float sample_rate);

        void setParameters(bool bypass_freeze, 
                           float feedback_pot_CV, 
                           float modulation_pot_CV, 
                           float lowpass_pot_CV, 
                           float highpass_pot_CV);

        float processReverb(float input);

        void modulationStage();

    private:
        AllPassFilter_Schroeder apf_s_;
        float apf_gain_;
        AllPassFilter_Schroeder apf_s_long_;
        float apf_gain_long_;
        FeedbackDelayNetwork fdn_;
        float hadamard_lengths_[DELAY_LINE_NUM];
        LowPassFilter_OnePole lpf_o_;
        HighPassFilter_OnePole hpf_o_;
        LFO lfo_;

        float processDiffusionStage(float input);
        float apf_mixed_;
        float processFDNStage(float input);
        float feedback_pot_CV_; 
        bool bypass_freeze_;
        float modulation_pot_CV_;
};

#endif
#ifndef REVERB_CONSTANTS_H
#define REVERB_CONSTANTS_H

/* REVERB CONSTANTS */
constexpr unsigned int DELAY_LINE_NUM = 4U;
constexpr unsigned int DELAY_LINE_NUM_AP = 12U;
constexpr size_t AP_MAX_DELAY = 7000U;
constexpr size_t FDN_MAX_DELAY = 45000U;

/* FILTER CONSTANTS */
constexpr float MAX_CUTOFF_LP = 10000.0f;
constexpr float MIN_CUTOFF_LP = 800.0f;
constexpr float MAX_CUTOFF_HP = 800.0f;
constexpr float MIN_CUTOFF_HP = 20.0f;
constexpr float lp_cutoff_ratio = MAX_CUTOFF_LP / MIN_CUTOFF_LP;
constexpr float hp_cutoff_ratio = MAX_CUTOFF_HP / MIN_CUTOFF_HP;
constexpr float modulation_amplitude_ratio = FDN_MAX_DELAY / 150.0f;

/* UTILS & OTHERS */
enum potentiometers
{
    dry_wet_pot,
    feedback_pot,
    lowpass_pot,
    highpass_pot,
    modulation_pot,
    ADC_NUM
};

#endif
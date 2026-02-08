#ifndef SW_UTILS_H
#define SW_UTILS_H

#include "daisysp.h"
#include <math.h>

/*
 * @brief: Class contains the needed functions and variables for Low-frequency oscillator (LFO).
 *
 * @note: Use case
 *        - Modulating parameters such as filter cutoff, pitch, or effect depth.
 */
class LFO
{
public:
    LFO() {}
    ~LFO() {}
    
    /*
     * @brief: Initializes the Low-frequency oscillator (LFO).
     *
     * @param frequency: Number of complete cycles per second.
     */
    void initLFO(float frequency);

    /*
     * @brief: Amplitude setter.
     */
    void setAmplitude(float new_amplitude);

    /*
     * @brief: Sample rate  setter.
     */
    void setSampleRate(float sample_rate);

    /*
     * @brief: Computes the current sine value based on its phase and amplitude. The output signal is used to
     *         modulate parameters of an audio processing system rather than producing audible output directly.
     *
     * @return: The current (phase incremented) LFO signal.
     */
    float process();

private:
    float sample_rate_;
    float phase_;
    float amplitude_;
    float frequency_;
};

float checkGain(float gain);
#endif
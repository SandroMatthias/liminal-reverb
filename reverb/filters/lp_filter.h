#ifndef LP_FILTER_H
#define LP_FILTER_H

#include "daisysp.h"
#include <math.h>

/*
 * @brief: Class contains the needed functions and variables to create
 *         a classic one-pole IIR low-pass filter in time domain.
 */
class LowPassFilter_OnePole
{
    public:
        LowPassFilter_OnePole(){}
        ~LowPassFilter_OnePole(){}

        /*
         * @brief: Initializes the one-pole low-pass filter by 
         *         setting the system sample rate.
         * 
         * @param sample_rate: Daisy Seed's sample rate.
         */
        void init(float sample_rate);

        /*
         * @brief: Sets the low-pass filter coefficient:
         *         Calculates the desired filter coefficient 'g' from the cutoff 
         *         frequency 'freq_cutoff' and sample rate 'sample_rate'.
         * 
         * @param freq_cutoff: Desired cutoff frequency, ideally set by a potentiometer.
         */
        void setCutoff(float freq_cutoff);
        
        /*
         * @brief: Applies smoothing  to the input signal by mixing the current input
         *         with the previous output, controlled by the feedback coefficient.
         *         Output should slowly follow the input.
         *   
         * @equation: y[n] = g * x[n] + (1 − g) * y[n − 1]
         *
         * @param input: Current input sample.
         * 
         * @return: The low-pass filtered output.
         */
        float processLowPass(float input);

    private:
        float lp_y_;
        static float sample_rate_;
        static float g_;
};
#endif
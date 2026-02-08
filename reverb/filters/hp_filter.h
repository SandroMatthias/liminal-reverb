#ifndef HP_FILTER_H
#define HP_FILTER_H

#include "daisysp.h"
#include <math.h>

/*
 * @brief: Class contains the needed functions and variables to create 
 *         a classic one-pole IIR high-pass filter in time domain.
 */
class HighPassFilter_OnePole
{
    public:
        HighPassFilter_OnePole(){}
        ~HighPassFilter_OnePole(){}

        /*
         * @brief: Initializes the one-pole high-pass filter 
         *         by setting the system sample rate.
         * 
         * @param sample_rate: Daisy Seed's sample rate.
         */
        void init(float sample_rate);

        /*
         * @brief: Sets the high-pass filter coefficient:
         *         Calculates the desired filter coefficient 'g' from the cutoff 
         *         frequency 'freq_cutoff' and sample rate 'sample_rate'.
         *         Only pass rapid changes.
         *         Remove the 'DC' or slow-moving part of the signal. 
         * 
         * @equation: y[n] = a * (y[n − 1] + x[n] − x[n − 1])
         * 
         * @param freq_cutoff: Desired cutoff frequency, ideally set by a potentiometer.
         */
        void setCutoff(float freq_cutoff);

        /*
         * @brief: Applies smoothing  to the input signal by mixing the current input
         *         with the previous output, controlled by the feedback coefficient.
         * 
         * @param input: Current input sample.
         * 
         * @return: The high-pass filtered output.
         */
        float processHighPass(float input);

    private:
        float hp_input_, hp_input_prev_;
        float hp_y_;
        static float g_, sample_rate_;     
};

#endif
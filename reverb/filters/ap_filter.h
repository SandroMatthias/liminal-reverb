#ifndef AP_FILTER_H
#define AP_FILTER_H

#include "daisy_seed.h"
#include "daisysp.h"
#include "../include/reverb_constants.h"
#include "../utils/sw_utils.h"

using namespace daisysp;

/*
 * @brief: Class contains the needed functions and variables for delay-based first-order Schroeder allpass filter. 
 *         This filter applies a phase rotation (where in its cycle it currently is == its starting point).
 *         Internally it uses a delay line(s) and a feedback coefficient 'g' to generate a series of decaying echoes.
 *          
 * @note: Use case
 *        (pre)diffuser for FDN.              
 */
class AllPassFilter_Schroeder
{
    public:
        AllPassFilter_Schroeder(){}
        ~AllPassFilter_Schroeder(){}

        /*
         * @brief: Initializes the schroder N stage allpass filter delay lines.
         *
         * @param ap_lengths: Reference to an array of size DELAY_LINE_NUM_AP, containing the delay line
         *                    lengths for each allpass delay line. The function will clamp each length
         *                    to  MAX_DELAY - 1 if necessary
         */
        void init(unsigned int (&ap_lengths)[DELAY_LINE_NUM_AP]);

        /*
         * @brief: Calculates and modifies the input sample via the Schroeder allpass equation.  
         * 
         *         Differential equation: y[n] = − g * x[n] + x[n − D] + g * y[n − D]
         * 
         *         - x[n]                  --> input
         *         - x[n - 1] & y[n - 1]   --> via delay buffer (prev in and prev output)
         *         - g                     --> gain
         *         - y[n]                  --> out
         *
         * @param input: Current input sample.
         * 
         * @param gain:  controls how much of the delayed signal 
         *               fed back - the decay time of the filter.
         * 
         * @important: gain must be in the range of 0 < |g| < 1
         * 
         * @return: Processed output value from the allpass filter.
         */
        float processAllpass(float input, float gain);

    private:
        static daisysp::DelayLine<float, AP_MAX_DELAY> ap_delay_[DELAY_LINE_NUM_AP];
};
#endif
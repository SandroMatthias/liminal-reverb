#include "ap_filter.h"

daisysp::DelayLine<float, AP_MAX_DELAY> DSY_SDRAM_BSS AllPassFilter_Schroeder::ap_delay_[DELAY_LINE_NUM_AP];

void AllPassFilter_Schroeder::init(unsigned int (&ap_lengths)[DELAY_LINE_NUM_AP])
{                          
    unsigned int checked_length;
    for(size_t n = 0; n < DELAY_LINE_NUM_AP; n++)
    {
        checked_length = (ap_lengths[n] < AP_MAX_DELAY) ? ap_lengths[n] : (AP_MAX_DELAY - 1);
        ap_delay_[n].Init();
        ap_delay_[n].SetDelay((float)checked_length);
    }
}

float AllPassFilter_Schroeder::processAllpass(float input, float gain)
{
    float delayed;
    float buf;
    float output;
    gain = checkGain(gain);
    for(size_t n = 0; n < DELAY_LINE_NUM_AP; n++)
    {
        delayed =  ap_delay_[n].Read();                     
        buf = input - gain * delayed;     
        output = delayed + gain * buf;       
        ap_delay_[n].Write(buf);
        input = output;                                    
    }
    return output;
}
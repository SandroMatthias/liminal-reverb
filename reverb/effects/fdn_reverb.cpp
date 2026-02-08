#include "fdn_reverb.h"

daisysp::DelayLine<float, FDN_MAX_DELAY> DSY_SDRAM_BSS FeedbackDelayNetwork::hadamard_delay_lines[DELAY_LINE_NUM];
daisysp::DelayLine<float, FDN_MAX_DELAY> DSY_SDRAM_BSS FeedbackDelayNetwork::householder_delay_lines[DELAY_LINE_NUM];

void FeedbackDelayNetwork::initHadamard(unsigned int(&hadamard_lengths)[DELAY_LINE_NUM], float sample_rate)
{
    static_assert(DELAY_LINE_NUM > 0, "DELAY_LINE_NUM must be greater than 0!");
    
    unsigned int checked_length;
    for(size_t n = 0; n < DELAY_LINE_NUM; n++)
    {
        checked_length = (hadamard_lengths[n] < FDN_MAX_DELAY - 1) ? hadamard_lengths[n] : (FDN_MAX_DELAY - 1);
        hadamard_delay_lines[n].Init();
        hadamard_delay_lines[n].SetDelay((float)checked_length);
        lpf_hadamard[n].init(sample_rate);
        hpf_hadamard[n].init(sample_rate);
    }
}

void FeedbackDelayNetwork::initHouseholder(unsigned int(&householder_lengths)[DELAY_LINE_NUM], float sample_rate)
{
    static_assert(DELAY_LINE_NUM > 0, "DELAY_LINE_NUM must be greater than 0!");

    unsigned int checked_length;
    for(size_t n = 0; n < DELAY_LINE_NUM; n++)
    {
        checked_length = (householder_lengths[n] < FDN_MAX_DELAY - 1) ? householder_lengths[n] : (FDN_MAX_DELAY - 1);
        householder_delay_lines[n].Init();
        householder_delay_lines[n].SetDelay((float)checked_length);
        lpf_householder[n].init(sample_rate);
        hpf_householder[n].init(sample_rate);
    }
}


float FeedbackDelayNetwork::processFDNHadamard(float input, float gain, bool freeze)
{   
    float output = 0.0f;
    for(size_t y = 0; y < DELAY_LINE_NUM; y++)
    {
        float sum = 0.0f;
        hadamard_read_[y] = hadamard_delay_lines[y].Read();                                           
		if(freeze)
        {
            hadamard_delay_lines[y].Write(hadamard_read_[y]);
            output += hpf_hadamard[y].processHighPass(lpf_hadamard[y].processLowPass(hadamard_read_[y]));
        }
        else
        {
            for(size_t x = 0; x < DELAY_LINE_NUM; x++)
            {
                sum += HADAMARD_MATRIX_[y][x] * lpf_hadamard[x].processLowPass(hadamard_read_[x]);			
            }
            hadamard_buffer_[y] = hpf_hadamard[y].processHighPass(sum * hadamard_scale_);	
            hadamard_delay_lines[y].Write((hadamard_buffer_[y] * gain) + input);			
            output += hadamard_read_[y];										       											
        }
    }
    if(freeze)
    {
        output *= 0.4f;
    }
    else
    {
         output /= DELAY_LINE_NUM;
    }
    return output;
}

float FeedbackDelayNetwork::getHadamardDelayTaps()
{
    float delay_taps =  0.41f * hadamard_delay_lines[0].Read(3500.0f) +
                        0.33f * hadamard_delay_lines[1].Read(2000.0f) +
                        0.12f * hadamard_delay_lines[2].Read(250.0f) +
                        0.23f * hadamard_delay_lines[3].Read(4000.0f) +
                        0.41f * hadamard_delay_lines[0].Read(8000.0f) +
                        0.33f * hadamard_delay_lines[1].Read(150.0f) +
                        0.12f * hadamard_delay_lines[2].Read(20000.0f) +
                        0.23f * hadamard_delay_lines[3].Read(11250.0f);
    return delay_taps;
}

float FeedbackDelayNetwork::processFDNHousholder(float input, float gain)
{
    float output = 0.0f;
    for(size_t y = 0; y < DELAY_LINE_NUM; y++)
    {
        float sum = 0.0f;
        householder_read_[y] = householder_delay_lines[y].Read();
        for(size_t x = 0; x < DELAY_LINE_NUM; x++)
        {
            sum += HOUSEHOLDER_MATRIX[y][x] * lpf_householder[x].processLowPass(householder_read_[x]);
        }
        householder_buffer_[y] = hpf_householder[y].processHighPass(sum);
        householder_delay_lines[y].Write((householder_buffer_[y] * gain) + input);
        output += householder_read_[y];
    }
    return output;
}

float FeedbackDelayNetwork::getHouseholderDelayTaps()
{
    float delay_taps =  0.41f * householder_delay_lines[0].Read(100.0f) +
                        0.33f * householder_delay_lines[1].Read(3000.0f) +
                        0.12f * householder_delay_lines[2].Read(6000.0f) +
                        0.23f * householder_delay_lines[3].Read(10000.0f) +
                        0.41f * householder_delay_lines[0].Read(1900.0f) +
                        0.33f * householder_delay_lines[1].Read(12500.0f) +
                        0.12f * householder_delay_lines[2].Read(33548.0f) +
                        0.23f * householder_delay_lines[3].Read(40000.0f);
    return delay_taps;
}
#include "daisysp.h"
#include "pitch_shifter.h"
#include <math.h>

void PitchShifter_SOLA::Init(float semitone)
{
        write_idx = 0;
        read_idx1_ = 0.0f;
        read_idx2_ = HOP_SIZE;            
        grain_idx1_ = 0;
        grain_idx2_ = HOP_SIZE;

        pitchFactor_ = pow(2, (semitone/12.0f)); 
        interpolation_ = false;

        if(pitchFactor_ != (int)pitchFactor_) //if non_integer
        {
                interpolation_ = true;
        }

        for(size_t i = 0; i < BUFFER_SIZE; i++)
        {
                buffer_[i] = 0.0f;
        }

        for(size_t j = 0; j < WINDOW_FRAME; j++)
        {
                hann_window_[j] = 0.5f * (1 - cosf((2 * M_PI * j) / (WINDOW_FRAME - 1)));
        }
}

/* writePos_ moves forward every sample -> read position move a different speed --> pitch shift */
void PitchShifter_SOLA::writeBuffer(float input_sample)
{
        buffer_[write_idx] = input_sample;
        write_idx = (write_idx + 1) % BUFFER_SIZE;      //MODULO WRAP
}

float PitchShifter_SOLA::Interpolate(float index)
{
        unsigned int index_current = (int)index; 
        unsigned int index_next = (index_current + 1) % BUFFER_SIZE; 
        float fraction = index - index_current; // [0-1] fractional part

        float interpolation = buffer_[index_current] * (1.0f - fraction) + buffer_[index_next] * fraction; //linear interpolation: y = y1 + (y2-y1)/(x2-x1) * (x - x1) */

        return interpolation; //fractional position
}

int PitchShifter_SOLA::searchBestLag(float idx1, float idx2)
{
        int best_lag = 0;
        float max_correlation = 0.0f;

       for(unsigned int lag = -MAX_LAG; lag <= MAX_LAG; lag++)
        {
                double correlation = 0.0f;
                for(size_t i = 0; i < OVERLAP_SIZE; i++)
                {
                        int idx1_tmp = (int)(idx1 + i) % BUFFER_SIZE;
                        int idx2_tmp = (int)(idx2 + i + lag) % BUFFER_SIZE;
                        correlation += buffer_[idx1_tmp] * buffer_[idx2_tmp];
                }

                if(correlation > max_correlation)
                {
                        max_correlation = correlation;
                        best_lag = lag;
                }
        }
        return best_lag;
}

float PitchShifter_SOLA::processSample(float input_sample)
{
        // 1. Write to buffer
        writeBuffer(input_sample);

        // 2. reading into the 2 read headers + applying the Hanning window
        if(interpolation_) //To avoid clicks: Only needed when the pitchFactor_ is a non-integer value (e.g. +7 semitones == 1.4983)
        {
                sample1 = Interpolate(read_idx1_);
                sample2 = Interpolate(read_idx2_);
        }
        else
        {
                sample1 = buffer_[(int)read_idx1_];
                sample2 = buffer_[(int)read_idx2_];
        }
        sample1 *= hann_window_[grain_idx1_];
        sample2 *= hann_window_[grain_idx2_];
        float pitched_sample = sample1 + sample2;

        //3. Incrementing the read positions (playback speed) and grain indices
        read_idx1_ += pitchFactor_; //"resampling"
        read_idx2_ += pitchFactor_;
        grain_idx1_++; //REFERENCE
        grain_idx2_++; //OVERLAPPING

        //4. Searching for the best lag + restarting the read headers
        if(grain_idx1_ >= WINDOW_FRAME)
        {
                grain_idx1_ = 0;
                read_idx1_ += HOP_SIZE;
                read_idx1_ = fmod(read_idx1_, BUFFER_SIZE);
        }
        if(grain_idx2_ >= WINDOW_FRAME)
        {
                grain_idx2_ = 0;
                read_idx2_ += HOP_SIZE;
                read_idx2_ = fmod(read_idx2_, BUFFER_SIZE); 
                if(pitchFactor_ < 1.0f)
                {
                        int bestLag = searchBestLag(read_idx1_, read_idx2_);
                        read_idx2_ += bestLag;
                        float max_read = write_idx + BUFFER_SIZE - HOP_SIZE;
                if(read_idx2_ > max_read)
                {
                        read_idx2_ = max_read;
                }
                }
                read_idx2_ = fmod(read_idx2_, BUFFER_SIZE);
                 
        }

        return pitched_sample;
}       

/*

LENGTH OF DELAY BUFFER:

- Depends on the maximum pitch and the grain/window size (OLA)
        - pitchFactor > 1 -> reads faster -> higher pitch
        - pitchFactor < 1 -> reads slower -> lower pitch

- Minimum buffer size: buffer_size = max_grain_size / pitchFactor
        EXAMPLE:
        - Lowest pitch = -12 -> pitchFactor = 0.5f;
        - Grain size = 1025 samples
        - Then buffer must be at least = 1024 / 0.5 = 2048 samples.

        MY CALC:
        - For shimmer reverb pedal -> I need octave 1 octave up -> pitchFactor = 2.0f
        - Grain size = 1024
        - Buffer = 1024 / 2 = 512 samples
        - 2048 samples / 48kHz ~ 43ms  (0,60ms = 0.06s -> 0.06s * 48kHz (samplerate) = 2 880 samples
        - 

//Interpolation: Read audio at fractional (non-integer) positions in the delay buffer ->
        //               Blends 2 neighboring samples
        //Like measuring a point between two dots on a line → smooth within a grain
        //Without it: clicks and aliasing
//Window: Fade in and out each grain smoothly;
        //        Shapes the amplitude envelope of the grain
        //like gently turning up and down the volume of the whole grain → smooth between grains
        //Without it: Clicks at the start and end of grain

//Crossfade: Smooth output without clicks when one sample reaches the buffer end
        //Cosine crossfade: y = s1 * (1 - f) + s2 * f
        //                  fade: f = 0.5 * (1 - cos(2 * PI * phase)
        //                  phase: normalized position in the grain
        //normalised phase position: represents where you are inside a grain as a number between 0-1: 
        //                           phase = current sample index in grain / grain size

*/
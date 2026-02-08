#pragma once
#ifndef PITCH_SHIFTER_H
#define PITCH_SHIFTER_H

#define BUFFER_SIZE 8192U       //Circular buffer size
#define WINDOW_FRAME 512U      //Grain size
#define HOP_SIZE (WINDOW_FRAME/4)           //grain step
#define MAX_LAG 128U            //sample search frame
#define OVERLAP_SIZE (WINDOW_FRAME - HOP_SIZE)       //WINDOW_FRAME - HOP_SIZE -> portion to correlate

/*
 * SOLA - Synchronous OverLap-Add
 * Time-domain algorithm used to change pitch without changing duration
 * 
 * 2 steps:
 *  I. Time-stretch or compress the audio
 *     - Duplicate or skip short segments of audio -> changes the length, but not the spectral content
 *  II. Resample to correct duration
 *      - After time-stretching/compressing -> signal is different in length
 *      - Fix: resample to the original length -> this changes the pitch
 * 
 */

class PitchShifter_SOLA
{
    public:
        void Init(float semitone);

        void writeBuffer(float input_sample); //X

        float Interpolate(float position);

        int searchBestLag(float position1, float position2);

        float processSample(float input_sample);

    private:
        unsigned int write_idx; 
        float read_idx1_, read_idx2_;                //Position in the circular buffer - 2 starts x time later to creat overlap
        unsigned int grain_idx1_, grain_idx2_; //Poistion inside the current grain window (0 -> WINDOW_FRAME-1) 
                                                   //grain_index2 -> hann window staggered correctly
        float pitchFactor_;
        bool interpolation_;
        float buffer_[BUFFER_SIZE];
        float hann_window_[WINDOW_FRAME];
        float sample1, sample2;
};

#endif
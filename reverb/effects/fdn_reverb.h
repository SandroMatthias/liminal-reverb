#ifndef FDN_REVERB_H
#define FDN_REVERB_H

#include "daisy_seed.h"
#include "daisysp.h"
#include "../filters/lp_filter.h"
#include "../filters/hp_filter.h"
#include "../include/reverb_constants.h"
#include <math.h>


/*
 * @brief: Class contains the necessary functions and variables for 
 * 		   creating a functional N X N FDN. 
 */
class FeedbackDelayNetwork
{
	public:
		FeedbackDelayNetwork()
		{   
			hadamard_scale_ = (1.0f)/sqrt((float)DELAY_LINE_NUM); //Needed for energy preservation
		}
		~FeedbackDelayNetwork(){}
	
		static daisysp::DelayLine<float, FDN_MAX_DELAY> hadamard_delay_lines[DELAY_LINE_NUM];
		static daisysp::DelayLine<float, FDN_MAX_DELAY> householder_delay_lines[DELAY_LINE_NUM];
		LowPassFilter_OnePole lpf_hadamard[DELAY_LINE_NUM];
		HighPassFilter_OnePole hpf_hadamard[DELAY_LINE_NUM];
		LowPassFilter_OnePole lpf_householder[DELAY_LINE_NUM];
		HighPassFilter_OnePole hpf_householder[DELAY_LINE_NUM];

	    /*
		 * @brief: Initializes the FDN delay lines and the filters.
		 *
		 * @param hadamard_lengths: Reference to an array of size DELAY_LINE_NUM, containing the delay line
		 *                    	    lengths for each FDN delay line. The function will clamp each length
		 *                	        to  MAX_DELAY - 1 if necessary.
		 *
		 * @param sample_rate: Daisy Seed's sample rate.
		 */
		void initHadamard(unsigned int(&hadamard_lengths)[DELAY_LINE_NUM], float sample_rate);

		/*
		 * @brief: Processes a sample through N X N hadamard FDN.
		 *		    
		 *         Reads the current outputs of all delay lines --> mixes them using Hadamard 
		 *		   matrix and applies feedback scaling. Writes the new modulated input + 
		 * 		   feedback buffer into each delay line with a loss factor. Sums all delay line
		 * 		   output and normalizes the summed value.
		 *
		 * @param input: Input signal of the feedback delay network.
		 *
		 * @param gain: Controls how much of the delayed signal 
         *              is fed back - the decay time.
		 * 
		 * @param freeze: Controls the freeze functionality. Freeze funcionality stops updating the
		 *				 reverb input while allowing the reverb tail to sustain indefinitely.
		 * 
		 * @returns: Combined FDN output.
		 */
		float processFDNHadamard(float input, float gain, bool freeze);

		/*
		 * @brief: Gets the delayed samples instantly from a specific point in time.
		 *
		 * @note: Good for mixing and addig early reflections to the signal. 
		 * 
		 * @return: Returns the mixed (summed and damped) taps. 
		 */
		float getHadamardDelayTaps();

		/*
		 * @brief: Initializes the FDN delay lines and the filters.
		 *
		 * @param householder_lengths: Reference to an array of size DELAY_LINE_NUM, containing the delay line
		 *                   		   lengths for each FDN delay line. The function will clamp each length
		 *                  		   to  MAX_DELAY - 1 if necessary.
		 *
		 * @param sample_rate: Daisy Seed's sample rate.
		 */
		void initHouseholder(unsigned int(&householder_lengths)[DELAY_LINE_NUM], float sample_rate);

		/*
		 * @brief: Processes a sample through N X N householder FDN.
		 *		    
		 *         Reads the current outputs of all delay lines --> mixes them using householder 
		 *		   matrix and applies feedback scaling. Writes the new modulated input + 
		 * 		   feedback buffer into each delay line with a loss factor. Sums all delay line
		 * 		   output.
		 *
		 * @param input: Input signal of the feedback delay network.
		 *
		 * @param gain: Controls how much of the delayed signal 
         *                  fed back - the decay time.
		 * 
		 * @returns: Combined FDN output.
		 */
		float processFDNHousholder(float input, float gain);

		/*
		 * @brief: Gets the delayed samples instantly from a specific point in time.
		 *
		 * @note: Good for mixing and addig early reflections to the signal. 
		 * 
		 * @return: Returns the mixed (summed and damped) taps. 
		 */
		float getHouseholderDelayTaps();

	private:
		const signed char HADAMARD_MATRIX_[DELAY_LINE_NUM][DELAY_LINE_NUM] = { {  1,  1,  1,  1 },
																		       {  1, -1,  1, -1 },
																			   {  1,  1, -1, -1 },
																			   {  1, -1, -1,  1 } };
		float hadamard_scale_;	
		float hadamard_read_[DELAY_LINE_NUM];
		float hadamard_buffer_[DELAY_LINE_NUM];
		const float HOUSEHOLDER_MATRIX[DELAY_LINE_NUM][DELAY_LINE_NUM] = { {  0.5f, -0.5f, -0.5f, -0.5f },
																		   { -0.5f,  0.5f, -0.5f, -0.5f },
																		   { -0.5f, -0.5f,  0.5f, -0.5f },
																		   { -0.5f, -0.5f, -0.5f,  0.5f } };
		float householder_read_[DELAY_LINE_NUM];
		float householder_buffer_[DELAY_LINE_NUM];
};
#endif
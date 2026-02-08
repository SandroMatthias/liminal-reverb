#include "daisy_seed.h"
#include "daisysp.h"
#include "liminal_reverb/reverb_effect.h"

using namespace daisy;
using namespace daisysp;
DaisySeed hw;

float input, output;
float sample_rate;

Reverb liminal_reverb;
PotentiometerUtil hwUtil;

/* REVERB/FILTER DELAY LINE EXPLICIT VALUES */
unsigned int householder_lengths_arr[] = {2000U, 13458U, 35837U, 41863U};

unsigned int apf_lengths_arr[DELAY_LINE_NUM_AP] = {113U, 179U, 271U, 359U,
												   463U, 557U, 653U, 761U,
												   887U, 1031U, 1201U, 1429U};

unsigned int apf_lengths_long_arr[DELAY_LINE_NUM_AP] = {1009U, 1483U, 1993U, 2503U,
														3023U, 3581U, 4127U, 4691U,
														5279U, 5843U, 6389U, 6997U};

unsigned int hadamard_lengths_arr[] = {8093U, 14235U, 21542U, 1251U};

/* CONTROL VOLTAGES */
float lowpass_pot_CV;
float highpass_pot_CV;
float feedback_pot_CV;
float modulation_pot_CV;
float dry_wet_pot_CV;

/* BUTTONS */
bool bypass_freeze = false;
Switch freezeSwitch;
GPIO led_freeze;

bool bypass_button = false;
Switch bypassSwitch;
GPIO led_on;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		liminal_reverb.setParameters(bypass_freeze, feedback_pot_CV, modulation_pot_CV, lowpass_pot_CV, highpass_pot_CV);
		input = in[0][i];
		if (bypass_button)
		{
			float reverb = liminal_reverb.processReverb(input);
			output = input * (1 - dry_wet_pot_CV) + 0.95 * reverb * dry_wet_pot_CV;
			out[0][i] = output;
			out[1][i] = output;
		}
		else
		{
			bypass_freeze = false;
			out[0][i] = input;
			out[1][i] = input;
		}
	}
	liminal_reverb.modulationStage();
}

void initRev()
{
	liminal_reverb.initReverb(hadamard_lengths_arr, householder_lengths_arr, apf_lengths_arr, apf_lengths_long_arr, 4.5f, sample_rate);
}

void initADC()
{
	AdcChannelConfig adc_config[ADC_NUM];
	adc_config[dry_wet_pot].InitSingle(daisy::seed::A3);
	adc_config[feedback_pot].InitSingle(daisy::seed::A2);
	adc_config[lowpass_pot].InitSingle(daisy::seed::A1);
	adc_config[highpass_pot].InitSingle(daisy::seed::A6);
	adc_config[modulation_pot].InitSingle(daisy::seed::A7);

	bypassSwitch.Init(daisy::seed::D19);
	led_on.Init(daisy::seed::D23, GPIO::Mode::OUTPUT);
	freezeSwitch.Init(daisy::seed::D20);
	led_freeze.Init(daisy::seed::D24, GPIO::Mode::OUTPUT);

	hw.adc.Init(adc_config, ADC_NUM);
	hw.adc.Start();
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	sample_rate = hw.AudioSampleRate();
	initRev();
	initADC();
	hw.SetLed(1); // it helps to see visually if the Daisy is in bootloader mode
	hw.StartAudio(AudioCallback);
	while (1)
	{
		/* CONTROL VOLTAGES */
		lowpass_pot_CV = MIN_CUTOFF_LP * pow(lp_cutoff_ratio, hwUtil.round(hw.adc.GetFloat(lowpass_pot)));
		highpass_pot_CV = MIN_CUTOFF_HP * pow(hp_cutoff_ratio, hwUtil.round(hw.adc.GetFloat(highpass_pot)));
		feedback_pot_CV = (hwUtil.round(hw.adc.GetFloat(feedback_pot)) / 100.0f) * 100.0f;
		modulation_pot_CV = hwUtil.round(hw.adc.GetFloat(modulation_pot)) * (modulation_amplitude_ratio);
		dry_wet_pot_CV = floor(hwUtil.round(hw.adc.GetFloat(dry_wet_pot)) * 100.0f) / 100.0f;

		/* BUTTONS & LEDS */
		bypassSwitch.Debounce();
		if (bypassSwitch.RisingEdge())
		{
			bypass_button = !bypass_button;
		}
		led_on.Write(bypass_button);

		freezeSwitch.Debounce();
		if (freezeSwitch.RisingEdge())
		{
			bypass_freeze = !bypass_freeze;
		}
		led_freeze.Write(bypass_freeze);
	}
}
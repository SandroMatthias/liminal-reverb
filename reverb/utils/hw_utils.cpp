#include "hw_utils.h"

float PotentiometerUtil::round(float potentiometer_value)
{
    if (potentiometer_value < MIN_POT_VALUE)
    {
        potentiometer_value = 0.0f;
    }
    else if (potentiometer_value > MAX_POT_VALUE)
    {
        potentiometer_value = 1.0f;
    }
    return potentiometer_value;
}
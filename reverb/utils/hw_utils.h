#ifndef HW_UTILS_H
#define HW_UTILS_H

class PotentiometerUtil
{
    public:
        float round(float potentiometer_value);

    private:
        const float MIN_POT_VALUE = 0.05f;
        const float MAX_POT_VALUE = 0.95f;
};

#endif
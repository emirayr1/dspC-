#include "distortion.h"
#include <cmath>

distortion::distortion(AudioFile<double> *audioFile, DistType _dist_type, double _rate, double _num_samples, int _num_channels)
    : audioFile(audioFile), dist_type(_dist_type), rate(_rate), num_samples(_num_samples), num_channels(_num_channels) // Initializer list
{
    calculate_dist();
}

distortion::~distortion()
{
}

void distortion::calculate_dist()
{
    switch (dist_type)
    {
    case DISTORTION_EXPONENTIAL:
        exponential_distortion();
        break;
    case DISTORTION_ARCTAN:
        arctan_distortion();
        break;
    case DISTORTION_CUBIC:
        cubic_distortion();
        break;
    case DISTORTION_NONE:
        none_distortion();
        break;
    case DISTORTION_PIECEWISE:
        piecewise_distortion();
    default:
        arctan_distortion();
        break;
    }
}

void distortion::arctan_distortion()
{
    int numSamplesPerChannel = audioFile->getNumSamplesPerChannel();
    int numChannels = audioFile->getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int i = 0; i < numSamplesPerChannel; ++i)
        {
            audioFile->samples[channel][i] = (2.0 / (std::atan(1.0) * 4.0 / 2.0)) * atan(audioFile->samples[channel][i] * rate); // M_PI ÇALIŞMADIGI ICIN ARCTAN ILE YAP
        }
    }
}

void distortion::exponential_distortion()
{
    int numSamplesPerChannel = audioFile->getNumSamplesPerChannel();
    int numChannels = audioFile->getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int n = 0; n < numSamplesPerChannel; ++n)
        {
            double sample = audioFile->samples[channel][n];
            double sign = std::signbit(sample) ? -1.0 : 1.0;
            audioFile->samples[channel][n] = sign * (1 - std::exp(-std::abs(rate * sample)));
        }
    }
}

void distortion::cubic_distortion()
{
    int numSamplesPerChannel = audioFile->getNumSamplesPerChannel();
    int numChannels = audioFile->getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int n = 0; n < numSamplesPerChannel; ++n)
        {
            double sample = audioFile->samples[channel][n];
            audioFile->samples[channel][n] = sample - rate * (1.0 / 3.0) * std::pow(sample, 3);
        }
    }
}

void distortion::piecewise_distortion()
{
    int numSamplesPerChannel = audioFile->getNumSamplesPerChannel();
    int numChannels = audioFile->getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int n = 0; n < numSamplesPerChannel; ++n)
        {
            double sample = audioFile->samples[channel][n];
            double absSample = std::abs(sample);

            if (absSample <= 1.0 / 3.0)
            {
                audioFile->samples[channel][n] = 2.0 * sample;
            }
            else if (absSample > 2.0 / 3.0)
            {
                audioFile->samples[channel][n] = std::signbit(sample) ? -1.0 : 1.0;
            }
            else
            {
                audioFile->samples[channel][n] = (std::signbit(sample) ? -1.0 : 1.0) * (3.0 - std::pow((2.0 - 3.0 * absSample), 2.0)) / 3.0;
            }
        }
    }
}

void distortion::none_distortion()
{
    // 
}

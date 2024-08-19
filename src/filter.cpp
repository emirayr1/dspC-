#include "filter.h"

Filter::Filter(FilterType _filter_type)
    : filter_type(_filter_type)
{
    set_filter_type(_filter_type);
}

Filter::~Filter()
{
}

void Filter::set_filter_type(FilterType _filter_type)
{
    filter_type = _filter_type;
    switch (filter_type)
    {
    case NONEE:
        delay_size = 0;
        break;
    case BLPF:
    {
        delay_size = 1;
        break;
    }
    case BHPF:
        delay_size = 1;
        break;
    case PCOMB:
        delay_size = 2;
        break;
    case NCOMB:
        delay_size = 2;
        break;
    default:
        delay_size = 0;
        break;
    }
}

void Filter::process(AudioFile<double> *audioFile)
{
    switch (filter_type)
    {
    case NONEE:
        return;
    case BLPF:
    {
        blpf(audioFile);
        break;
    }
    case BHPF:
        bhpf(audioFile);
        break;
    case PCOMB:
        pcomb(audioFile);
        break;
    case NCOMB:
        ncomb(audioFile);
        break;
    default:
        return;
    }
}

void Filter::blpf(AudioFile<double> *audioFile)
{
    for (size_t i = 0; i < audioFile->getNumChannels(); i++)
    {
        for (size_t j = 0; j < audioFile->getNumSamplesPerChannel(); j++)
        {
            if (j == 0)
            {
                buffer[i][0] = audioFile->samples[i][j];
                continue;
            }
            double temp = audioFile->samples[i][j];
            audioFile->samples[i][j] = temp + buffer[i][0]; // audioFile->samples[i][j] + temp;
            buffer[i][0] = temp;
        }
    }
}

void Filter::bhpf(AudioFile<double> *audioFile)
{
    for (size_t i = 0; i < audioFile->getNumChannels(); i++)
    {
        for (size_t j = 0; j < audioFile->getNumSamplesPerChannel(); j++)
        {
            if (j == 0)
            {
                buffer[i][0] = audioFile->samples[i][j];
                continue;
            }
            double temp = audioFile->samples[i][j];
            audioFile->samples[i][j] = temp - buffer[i][0]; // audioFile->samples[i][j] - temp;
            buffer[i][0] = temp;
        }
    }
}

void Filter::pcomb(AudioFile<double> *audioFile)
{
    for (size_t i = 0; i < audioFile->getNumChannels(); i++)
    {
        for (size_t j = 0; j < audioFile->getNumSamplesPerChannel(); j++)
        {
        }
    }
}

void Filter::ncomb(AudioFile<double> *audioFile)
{
}

#include <fade.h>

fade::fade(double _samplerate, double _fade_in_duration, double _fade_out_duration, FadeType _fade_in_type, FadeType _fade_out_type)
{
    sample_rate = _samplerate;
    fade_in_duration = _fade_in_duration;
    fade_out_duration = _fade_out_duration;
    fade_in_type = _fade_in_type;
    fade_out_type = _fade_out_type;
    fade_in_samples = new double[int(fade_in_duration * sample_rate)];
    fade_out_samples = new double[int(fade_out_duration * sample_rate)];
    calculate_curves();
}

fade::~fade()
{
    if (fade_in_samples != nullptr)
    {
        delete[] fade_in_samples;
    }
    if (fade_out_samples != nullptr)
    {
        delete[] fade_out_samples;
    }
}

void fade::calculate_curves()
{
    switch (fade_in_type)
    {
    case LINEAR:
        linear_fade_in();
        break;
    case EXPONENTIAL:
        exponential_fade_in();
        break;
    case SCURVE:
        scurve_fade_in();
        break;
    case EQUAL:
        equal_fade_in();
        break;
    case NONE:
        none_in();
        break;
    default:
        linear_fade_in();
        break;
    }
    switch (fade_out_type)
    {
    case LINEAR:
        linear_fade_out();
        break;
    case EXPONENTIAL:
        exponential_fade_out();
        break;
    case SCURVE:
        scurve_fade_out();
        break;
    case EQUAL:
        equal_fade_out();
        break;
    case NONE:
        none_out();
        break;
    default:
        linear_fade_in();
        break;
    }
}

void fade::linear_fade_in()
{
    int num_samples = int(fade_in_duration * sample_rate);
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_in_samples[i] = float(i) / float(num_samples);
    }
}
void fade::linear_fade_out()
{
    int num_samples = int(fade_out_duration * sample_rate);
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_out_samples[i] = 1.0 - (float(i) / float(num_samples));
    }
}

void fade::exponential_fade_in()
{
    int num_samples = int(fade_in_duration * sample_rate);
    double rate = 2.0;
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_in_samples[i] = 1.0 - (float(i) / float(num_samples));
        fade_in_samples[i] = 1.0 - std::pow(fade_in_samples[i], rate);
    }
}
void fade::exponential_fade_out()
{
    int num_samples = int(fade_out_duration * sample_rate);
    double rate = 2.0;
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_out_samples[i] = (float(i) / float(num_samples));
        fade_out_samples[i] = 1.0 - std::pow(fade_out_samples[i], rate);
    }
}

void fade::scurve_fade_in()
{
    int num_samples = int(fade_in_duration * sample_rate);
    double rate = 2.0;
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_in_samples[i] = float(i) / float(num_samples >> 1); // bir basamak sağa kaydır(1 bit) 2ye böler
        fade_in_samples[i] = std::fmod(fade_in_samples[i], 1.0);
        if (i < (num_samples >> 1))
        {
            fade_in_samples[i] = 0.5 * std::pow(fade_in_samples[i], rate);
        }
        else
        {
            fade_in_samples[i] = 0.5 * (1.0 - std::pow(1.0 - fade_in_samples[i], rate)) + 0.5;
        }
    }
}
void fade::scurve_fade_out()
{
    int num_samples = int(fade_out_duration * sample_rate);
    double rate = 2.0;
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_out_samples[i] = float(i) / float(num_samples >> 1); // bir basamak sağa kaydır(1 bit) 2ye böler
        fade_out_samples[i] = std::fmod(fade_out_samples[i], 1.0);
        if (i < (num_samples >> 1))
        {
            fade_out_samples[i] = 0.5 * (1.0 - std::pow(fade_out_samples[i], rate)) + 0.5;
        }
        else
        {
            fade_out_samples[i] = 0.5 * std::pow((1.0 - fade_out_samples[i]), rate);
        }
    }
}

void fade::equal_fade_in()
{
    int num_samples = int(fade_out_duration * sample_rate);
    double rate = 2.0;
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_in_samples[i] = float(i) / float(num_samples);
        fade_in_samples[i] = std::pow(fade_in_samples[i], (1.0 / rate));
    }
}
void fade::equal_fade_out()
{
    int num_samples = int(fade_out_duration * sample_rate);
    double rate = 2.0;
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_out_samples[i] = 1.0 - (float(i) / float(num_samples));
        fade_out_samples[i] = std::pow(fade_out_samples[i], 1.0 / rate);
    }
}

void fade::none_in()
{
    int num_samples = int(fade_out_duration * sample_rate);
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_in_samples[i] = 1.0;
    }
}
void fade::none_out()
{
    int num_samples = int(fade_out_duration * sample_rate);
    for (size_t i = 0; i < num_samples; i++)
    {
        fade_out_samples[i] = 1.0;
    }
}

// SET FUNCTIONS //
void fade::set_fade_in_duration(double _fade_in_duration)
{
    fade_in_duration = _fade_in_duration;
    if (fade_in_samples != nullptr)
    {
        delete[] fade_in_samples;
    }
    fade_in_samples = new double[int(fade_in_duration * sample_rate)];
    calculate_curves();
}
void fade::set_fade_out_duration(double _fade_out_duration)
{
    fade_out_duration = _fade_out_duration;
    if (fade_out_samples != nullptr)
    {
        delete[] fade_out_samples;
    }
    fade_out_samples = new double[int(fade_out_duration * sample_rate)];
    calculate_curves();
}

void fade::set_fade_in_type(FadeType _fade_in_type)
{
    fade_in_type = _fade_in_type;
    calculate_curves();
}
void fade::set_fade_out_type(FadeType _fade_out_type)
{
    fade_out_type = _fade_out_type;
    calculate_curves();
}

// PROCESS //
void fade::process(double **samples, int num_samples, int num_channels)
{
    int fade_in_num_samples = int(fade_in_duration * sample_rate);
    int fade_out_num_samples = int(fade_out_duration * sample_rate);
    int fade_out_start_samples = num_samples - fade_out_num_samples;

    for (size_t i = 0; i < num_channels; i++)
    {
        for (size_t j = 0; j < fade_in_num_samples; j++)
        {
            samples[i][j] *= fade_in_samples[j];
        }
        for (size_t j = fade_out_start_samples; j < num_samples; j++)
        {
            samples[i][j] *= fade_out_samples[j];
        }
    }
}

void fade::process(std::vector<std::vector<double>> *samples, int num_samples, int num_channels)
{
    int fade_in_num_samples = int(fade_in_duration * sample_rate);
    int fade_out_num_samples = int(fade_out_duration * sample_rate);
    int fade_out_start_samples = num_samples - fade_out_num_samples;

    for (size_t i = 0; i < num_channels; i++)
    {
        for (size_t j = 0; j < fade_in_num_samples; j++)
        {
            samples->at(i)[j] *= fade_in_samples[j];
        }
        for (size_t j = fade_out_start_samples; j < num_samples; j++)
        {
            samples->at(i)[j] *= fade_out_samples[j - fade_out_start_samples];
        }
    }
}

void fade::process(AudioFile<double> *a_file, int num_samples, int num_channels)
{
    int fade_in_num_samples = int(fade_in_duration * sample_rate);
    int fade_out_num_samples = int(fade_out_duration * sample_rate);
    int fade_out_start_samples = num_samples - fade_out_num_samples;

    for (size_t i = 0; i < num_channels; i++)
    {
        for (size_t j = 0; j < fade_in_num_samples; j++)
        {
            a_file->samples[i][j] *= fade_in_samples[j];
        }
        for (size_t j = fade_out_start_samples; j < num_samples; j++)
        {
            a_file->samples[i][j] *= fade_out_samples[j];
        }
    }
}
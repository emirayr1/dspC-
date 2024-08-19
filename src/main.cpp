#include <iostream>
#include "AudioFile.h"
#include <fade.h>
#include <distortion.h>
#include "filter.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void soft_clip(AudioFile<double> *audio_file, double threshold)
{
    for (size_t i = 0; i < audio_file->getNumChannels(); i++)
    {
        for (size_t j = 0; j < audio_file->getNumSamplesPerChannel(); j++)
        {
            if (audio_file->samples[i][j] > threshold)
            {
                audio_file->samples[i][j] = threshold;
            }
            else if (audio_file->samples[i][j] < -threshold)
            {
                audio_file->samples[i][j] = -threshold;
            }
        }
    }
}
// NUMPY ZEROS_LIKE DENEME
double **zeros_like(size_t channel, size_t samples)
{

    // Dinamik Array Oluşturma
    double **zeros = new double *[channel];

    // Sıfır Yazdırma
    for (size_t i = 0; i < channel; i++)
    {
        zeros[i] = new double[samples];
        for (size_t j = 0; j < samples; j++)
        {
            zeros[i][j] = 0;
        }
    }

    return zeros;
}

double **pan(double *in_signal, int num_samples, double panValue, int panType)
{

    // RHYTMIC AUTO PAN EFFECT 4 //

    double panTransform = (panValue / 200.0) + 0.5;
    double leftAmp, rightAmp;

    // PAN TÜRLERİ //
    if (panType == 1)
    {
        leftAmp = 1.0 - panTransform;
        rightAmp = panTransform;
    }
    else if (panType == 2)
    {
        leftAmp = std::sqrt(1.0 - panTransform);
        rightAmp = std::sqrt(panTransform);
    }
    else if (panType == 3)
    {
        leftAmp = std::sin((1.0 - panTransform) * (std::atan(1.0) * 4.0 / 2.0));
        rightAmp = std::sin((panTransform) * (std::atan(1.0) * 4.0 / 2.0));
    }
    // OUTPUT SIGNAL
    double **outSignal = new double *[num_samples];
    for (size_t i = 0; i < num_samples; i++)
    {
        outSignal[i] = new double[2];
    }

    for (int i = 0; i < num_samples; i++)
    {
        outSignal[i][0] = leftAmp * in_signal[i];
        outSignal[i][1] = rightAmp * in_signal[i];
    }

    return outSignal;
}

void rtmpan(AudioFile<double> *audioFile, int frequency)
{ // function overload PAN
    int N = audioFile->getNumSamplesPerChannel();
    double Fs = audioFile->getSampleRate();
    double Ts = 1.0 / Fs;

    // LFO time vector
    double *t = new double[N];
    for (size_t i = 0; i < N; i++)
    {
        t[i] = i * Ts;
    }

    double *panValue = new double[N];
    for (size_t i = 0; i < N; i++)
    {
        panValue[i] = std::sin(2 * M_PI * frequency * t[i]);
    }

    for (size_t j = 0; j < audioFile->getNumChannels(); j++)
    {

        for (size_t i = 0; i < N; i++)
        {
            double left_gain = (1 - panValue[i]) / 2.0;
            double right_gain = (1 + panValue[i]) / 2.0;
            audioFile->samples[0][i] *= left_gain;
            audioFile->samples[1][i] *= right_gain;
        }
    }
    delete[] t;
    delete[] panValue;
}

// MAIN //
int main()
{
    AudioFile<double> a_file = AudioFile<double>();
    AudioFile<double> delay_file = AudioFile<double>();

    bool response_a = a_file.load("VIOLIN 1.wav");
    bool response_delay = delay_file.load("delay_deneme-converted24.wav");

    size_t numSamples_delay = delay_file.getNumSamplesPerChannel();

    // LEFT AND RIGHT CHANNELS ARRAY

    double *leftChannel = new double[numSamples_delay];
    double *rightChannel = new double[numSamples_delay];

    // LEFT AND RIGHT CHANNELS SAMPLES
    for (size_t j = 0; j < delay_file.getNumSamplesPerChannel(); j++)
    {
        leftChannel[j] = delay_file.samples[0][j];
        rightChannel[j] = delay_file.samples[1][j];
    }

    // CREATING MID-SIDES
    double *mid = new double[numSamples_delay];
    double *sides = new double[numSamples_delay];

    for (size_t j = 0; j < delay_file.getNumSamplesPerChannel(); j++)
    {
        mid[j] = 0.5 * (leftChannel[j] + rightChannel[j]);
        sides[j] = 0.5 * (leftChannel[j] - rightChannel[j]);
    }

    // creating width
    double width = 1;
    for (size_t j = 0; j < delay_file.getNumSamplesPerChannel(); j++)
    {
        mid[j] = mid[j] * (1.0 - width);
        sides[j] = sides[j] * width;
    }

    // WIDTH MID SIDES TO NEW LEFT AND RIGHT
    double *newL = new double[numSamples_delay];
    double *newR = new double[numSamples_delay];
    double *new_Mono = new double[numSamples_delay];

    for (size_t j = 0; j < delay_file.getNumSamplesPerChannel(); j++)
    {
        newL[j] = mid[j] + sides[j];
        newR[j] = mid[j] - sides[j];
        new_Mono[j] = 0.5 * (newL[j] + newR[j]);
    }

    // NEW PANNED SIGNAL
    double **panned_signal = pan(new_Mono, delay_file.getNumSamplesPerChannel(), 80, 1);

    // DELAY CALCULATING
    double beats_per_min = 69.0;
    double beats_per_sec = beats_per_min / 60.0;
    double sec_per_beat = 1.0 / beats_per_sec;
    double noteDiv = 0.75;
    double delay_time_sec = noteDiv * sec_per_beat;
    int d = int(delay_time_sec * delay_file.getSampleRate());

    // RIGHT VE LEFT DELAY İÇİN AMPLTD KONTROLÜ
    double delay_amp_L = 1.0;
    double delay_amp_R = 0.33;

    if (response_delay)
    {
        for (size_t j = 0; j < delay_file.getNumSamplesPerChannel(); j++)
        {
            delay_file.samples[0][j] = panned_signal[j][0]; // Left channel    // BİRBİRLERİNE TERS
            delay_file.samples[1][j] = panned_signal[j][1]; // Right channel
        }

        delay_file.save("panli.wav");

        size_t numSampDelay = delay_file.getNumSamplesPerChannel();

        double **zeros = zeros_like(delay_file.getNumChannels(), numSampDelay);
        for (size_t n = 0; n < numSampDelay; n++)
        {
            zeros[0][n] = delay_file.samples[0][n];
            zeros[1][n] = delay_file.samples[1][n];
        }
        for (size_t n = 0; n < numSampDelay; n++)
        {
            if (n < d + 1)
            {

                delay_file.samples[0][n] = zeros[0][n];
                delay_file.samples[1][n] = zeros[1][n];
            }
            else
            {

                delay_file.samples[0][n] = zeros[0][n] + delay_amp_L * zeros[0][n - d];
                delay_file.samples[1][n] = zeros[1][n] + delay_amp_R * zeros[1][n - d];
            }
        }

        delay_file.save("delay_li_hali.wav");
        // FEEDBACK DELAY

        //  for(size_t n=0;n<numSampDelay;n++){
        //     if(n < d+1){
        //         //zeros[n] = delay_file.samples[0][n];
        //         delay_file.samples[i][n] = delay_file.samples[i][n];
        //     }
        //     else{
        //         //zeros[n] = delay_file.samples[0][n] + -a * zeros[n - d];
        //         delay_file.samples[i][n] = delay_file.samples[i][n] + a * delay_file.samples[i][n - d];
        //     }
        // }
        delete[] zeros;
        delete[] leftChannel;
        delete[] rightChannel;
        delete[] mid;
        delete[] sides;
        delete[] newL;
        delete[] newR;
        delete[] new_Mono;
        for (size_t i = 0; i < numSamples_delay; i++)
        {
            delete[] panned_signal[i];
        }
        delete[] panned_signal;
    }

    else
    {
        std::cout << "Delay dosyası yuklenemedi" << std::endl;
    }
    if (response_a)
    {
        std::cout << "Dosya yuklendi" << std::endl;
        double max = 0.0;
        std::cout << a_file.getNumChannels() << std::endl;

        for (size_t i = 0; i < a_file.getNumChannels(); i++)
        {
            for (size_t j = 0; j < a_file.getNumSamplesPerChannel(); j++)
            {
                if (std::abs(a_file.samples[i][j]) > max)
                {
                    max = std::abs(a_file.samples[i][j]);
                }
            }
        }
        for (size_t i = 0; i < a_file.getNumChannels(); i++)
        {
            for (size_t j = 0; j < a_file.getNumSamplesPerChannel(); j++)
            {
                a_file.samples[i][j] = a_file.samples[i][j] / max;
            }
        }

        // soft_clip(&a_file, 0.25);
        // fade fader = fade(a_file.getSampleRate(), 3.0, 3.0, NONE, EXPONENTIAL); // enum NONE ekle ve 1le doldur fonksiyonları yaz
        // fader.process(&a_file.samples, a_file.getNumSamplesPerChannel(), a_file.getNumChannels());

        /*



        DISTORTION



        */

        //distortion dist = distortion(&a_file, DISTORTION_ARCTAN, 10, a_file.getNumSamplesPerChannel(), a_file.getNumChannels());
        Filter filter = Filter(BLPF);
        filter.process(&a_file);

        // RYTHM PAN EFFECT FUNCION

        // rtmpan(&delay_file, 3);

        delay_file.save("rtm_delay_file.wav");
        a_file.save("filter_violin.wav");
        std::cout << "Dosya kaydedildi" << std::endl;
    }
    else
    {
        std::cout << "Dosya yuklenemedi" << std::endl;
    }

    return 0;
}
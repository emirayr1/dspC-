#include <cmath>
#include <vector>
#include "AudioFile.h"

enum FadeType
{
    LINEAR,
    EXPONENTIAL,
    SCURVE,
    EQUAL,
    NONE
};
class fade
{
private:
    double sample_rate;
    double fade_in_duration;
    double fade_out_duration;
    double *fade_in_samples;
    double *fade_out_samples;
    FadeType fade_in_type;
    FadeType fade_out_type;

    void linear_fade_in();
    void linear_fade_out();
    void exponential_fade_in();
    void exponential_fade_out();
    void scurve_fade_in();
    void scurve_fade_out();
    void equal_fade_in();
    void equal_fade_out();
    void none_in();
    void none_out();
    void calculate_curves();

public:
    fade(double _samplerate, double _fade_in_duration, double _fade_out_duration, FadeType _fade_in_type, FadeType _fade_out_type);
    ~fade();
    void set_fade_in_duration(double _fade_in_duration);
    void set_fade_out_duration(double _fade_out_duration);
    void set_fade_in_type(FadeType _fade_in_type);
    void set_fade_out_type(FadeType _fade_out_type);
    void process(double **samples, int num_samples, int num_channels);
    void process(std::vector<std::vector<double>> *samples, int num_samples, int num_channels);
    void process(AudioFile<double> *a_file, int num_samples, int num_channels); // function overload
};

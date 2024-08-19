#include "AudioFile.h"

enum DistType
{
    DISTORTION_EXPONENTIAL,
    DISTORTION_ARCTAN,
    DISTORTION_CUBIC,
    DISTORTION_PIECEWISE,
    DISTORTION_NONE
};

class distortion
{
public:
    distortion(AudioFile<double> *audioFile, DistType _dist_type, double _rate, double _num_samples, int _num_channels);
    ~distortion();

    
private:
    void calculate_dist();
    void exponential_distortion();
    void arctan_distortion();
    void cubic_distortion();
    void piecewise_distortion();
    void none_distortion();

    AudioFile<double> *audioFile;
    DistType dist_type;
    double rate;
    double num_samples;
    int num_channels;
};


#include "vector"
#include "AudioFile.h"

enum FilterType
{
    NONEE,
    BLPF,
    BHPF,
    PCOMB,
    NCOMB,
};

class Filter
{
    

    std::vector<std::vector<double>> buffer = std::vector<std::vector<double>>(2, std::vector<double>(10));
    

    int delay_size;
    FilterType filter_type;

    public:

        Filter(FilterType _filter_type);
        ~Filter();
        void set_filter_type(FilterType);
        void process(AudioFile<double> *audioFile);
        void blpf(AudioFile<double> *audioFile);
        void bhpf(AudioFile<double> *audioFile);
        void pcomb(AudioFile<double> *audioFile);
        void ncomb(AudioFile<double> *audioFile);
};

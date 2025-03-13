#ifndef WAV_FILE
#define WAV_FILE

#include <cstdio>

class WavFile {
    public:
        WavFile(const char *_path);
        ~WavFile();

        FILE *get_file() { return file; }
        int get_sample_rate() { return sample_rate; }
        short get_bits_per_sample() { return bits_per_sample; }
        const char *get_file_path() { return file_path; }
    protected:
        std::FILE *file;
        int sample_rate;
        short bits_per_sample;
        const char *file_path;
};

#endif
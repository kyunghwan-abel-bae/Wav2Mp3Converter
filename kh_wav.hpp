#include <iostream>
#include <vector>
#include <fstream>

class KHWav {
public:
    KHWav (const char* file_name);

    int32_t get_samples_per_sec() const;
    int32_t get_avg_bytes_per_sec() const;
    int16_t get_channels() const;

    int get_samples( unsigned int offset, unsigned int size, short int* samples) const;
    int get_samples( unsigned int offset, unsigned int size, char* samples) const;

    struct RIFF {
        char riffID[4];
        int32_t riffSIZE;
        char riffFORMAT[4];
    };

    struct FMTHDR {
        char fmtID[4];
        int32_t fmtSIZE;
    };

    struct FMT {
        int16_t wFormatTag;
        int16_t nChannels;
        int32_t nSamplesPerSec;
        int32_t nAvgBytesPerSec;
        int16_t nBlockAlign;
        int16_t wBitsPerSample;
    };
    
    struct DATA {
        char dataID[4];
        int32_t dataSIZE;
    };

    struct FACT {
        int32_t samplesNumber;
        int32_t t;
    };

public:
    static const int kRIFF_SIZE = 12;
    static const int kFMTHDR_SIZE = 8;
    static const int kFMT_SIZE = 16;
    static const int kDATA_SIZE = 8;
    static const int kFACT_SIZE = 8;

private:
    std::vector<char> wav_;

    RIFF                    riff_;
    FMTHDR                  fmthdr_;
    FMT                     fmt_;
    std::vector<char>       fmt_extra_bytes_;
    FACT                    fact_;
    DATA                    data_;
    unsigned int            extra_param_length_;
    std::vector<char>       extra_param_;
};

KHWav::KHWav (const char* file_name) {
    fmt_.wFormatTag = 0;
    extra_param_length_ = 0;
    fact_.samplesNumber = -1;

    std::ifstream file( file_name, std::ios_base::binary | std::ios_base::in );
    if( file.is_open() == false ) {
        std::cout << "Failed to open the file" << std::endl;
    }

    file.read((char*)&riff_, kRIFF_SIZE);
    file.read((char*)&fmthdr_, kFMTHDR_SIZE);
    file.read((char*)&fmt_, kFMT_SIZE);

    unsigned int fmt_extra_bytes = fmthdr_.fmtSIZE - kFMT_SIZE;

    if( fmt_extra_bytes > 0 ) {
        fmt_extra_bytes_.resize( fmt_extra_bytes );
        file.read( &fmt_extra_bytes_[0], fmt_extra_bytes );
    }

    if( fmt_.wFormatTag != 1 ) {
       file.read((char*)&extra_param_length_, 2); 

       if( extra_param_length_ > 0) {
           extra_param_.resize(extra_param_length_);
           file.read(&extra_param_[0], extra_param_length_);
       }
    }

    file.read((char*)&(data_.dataID), 4);

    if(data_.dataID[0] == 'f' && data_.dataID[1] == 'a' && data_.dataID[2] == 'c' && data_.dataID[3] == 't') {
        file.read( (char*)&fact_, kFACT_SIZE );
        file.read( (char*)&data_, kDATA_SIZE );
    }
    else
        file.read((char*)&(data_.dataSIZE), 4);

    wav_.resize( data_.dataSIZE );

    file.read( &wav_[0], data_.dataSIZE );
}

int32_t KHWav::get_samples_per_sec() const {
    return fmt_.nSamplesPerSec;
}

int32_t KHWav::get_avg_bytes_per_sec() const {
    return fmt_.nAvgBytesPerSec;
}

int16_t KHWav::get_channels() const {
    return fmt_.nChannels;
}

int KHWav::get_samples( unsigned int offset, unsigned int size, short int* samples) const {
    if( offset > (unsigned int)data_.dataSIZE)
        return 0;

    unsigned int real_size = (offset + size) < (unsigned int)data_.dataSIZE ? size : ((unsigned)data_.dataSIZE - offset);

    std::copy(&wav_[offset], &wav_[offset+real_size], samples);

    return real_size;
}

int KHWav::get_samples( unsigned int offset, unsigned int size, char* samples) const {
    if( offset > (unsigned int)data_.dataSIZE)
        return 0;

    unsigned int real_size = (offset + size) < (unsigned int)data_.dataSIZE ? size : ((unsigned)data_.dataSIZE - offset);

    std::copy(&wav_[offset], &wav_[offset+real_size], samples);

    return real_size;
}

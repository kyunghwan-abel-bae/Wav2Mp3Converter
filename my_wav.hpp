#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>

#include <fstream>
#include <errno.h>
#include <cstring>

class MyWav {
public:
    MyWav (const char* file_name);

    int32_t get_samples_per_sec() const;
    int32_t get_avg_bytes_per_sec() const;
    int16_t get_channels() const;

    void get_samples( unsigned int offset, unsigned int size, std::vector<char>& samples) const;

    struct RIFF
    {
        char riffID[4];     //4
        int32_t riffSIZE;   //4
        char riffFORMAT[4]; //4
    };

    struct FMTHDR
    {
        char fmtID[4];      //4
        int32_t fmtSIZE;    //4
    };

    struct FMT
    {
        int16_t wFormatTag; //2
        int16_t nChannels;  //2
        int32_t nSamplesPerSec;     //4
        int32_t nAvgBytesPerSec;    //4
        int16_t nBlockAlign;        //2
        int16_t wBitsPerSample;     //2
    };

    struct DATA
    {
        char dataID[4];     // 4 bytes
        int32_t dataSIZE;   // 4 bytes
    };
    struct FACT
    {
        int32_t samplesNumber;
        int32_t t;
    };

public:
    static const int RIFF_SIZE      = 12;
    static const int FMTHDR_SIZE    = 8;
    static const int FMT_SIZE       = 16;
    static const int DATA_SIZE      = 8;
    static const int FACT_SIZE      = 8;

private:
    std::vector<char> wave_;

    RIFF                    riff_;
    FMTHDR                  fmthdr_;
    FMT                     fmt_;
    std::vector<char>       fmt_extra_bytes_;
    FACT                    fact_;
    DATA                    data_;
    int16_t                 extra_param_length_;
    std::vector<char>       extra_param_;
};

MyWav::MyWav (const char* file_name) {
    fmt_.wFormatTag = 0;
    extra_param_length_ = 0;
    fact_.samplesNumber = -1;

    std::ifstream file( file_name, std::ios_base::binary | std::ios_base::in );
    if( file.is_open() == false ) {
        std::cout << "Failed to open the file" << std::endl;
    }

    file.read( reinterpret_cast<char*>( &riff_ ), RIFF_SIZE );
    file.read( reinterpret_cast<char*>( &fmthdr_ ), FMTHDR_SIZE );

    file.read( reinterpret_cast<char*>( &fmt_ ), FMT_SIZE );

    unsigned fmt_extra_bytes = fmthdr_.fmtSIZE - FMT_SIZE;

    if( fmt_extra_bytes > 0 ) {
        fmt_extra_bytes_.resize( fmt_extra_bytes );
        file.read( &fmt_extra_bytes_[0], fmt_extra_bytes );
    }

    if( fmt_.wFormatTag != 1) {
        file.read(reinterpret_cast<char*>(&extra_param_length_), 2);
        if( extra_param_length_ > 0) {
            extra_param_.resize(extra_param_length_);
            file.read(&extra_param_[0], extra_param_length_);
        }
    }

    file.read(reinterpret_cast<char*>( &data_.dataID ), 4);

    if( data_.dataID[0] == 'f' && data_.dataID[1] == 'a' && data_.dataID[2] == 'c' && data_.dataID[3] == 't') {
        file.read( reinterpret_cast<char*>(&fact_), FACT_SIZE );
        file.read( reinterpret_cast<char*>(&data_), DATA_SIZE ); 
    }
    else
        file.read( reinterpret_cast<char*>( &data_.dataSIZE ), 4);

    
    wave_.resize( data_.dataSIZE );

    file.read( &wave_[0], data_.dataSIZE );
}

int32_t MyWav::get_samples_per_sec() const {
    return fmt_.nSamplesPerSec;
}

int32_t MyWav::get_avg_bytes_per_sec() const {
    return fmt_.nAvgBytesPerSec;
}

int16_t MyWav::get_channels() const {
    return fmt_.nChannels;
}

void MyWav::get_samples( unsigned int offset, unsigned int size, std::vector<char>& samples) const {
    if( offset > (unsigned)data_.dataSIZE )
        return;

    unsigned int real_size = ( offset + size ) < ( unsigned ) data_.dataSIZE ? size : ((unsigned) data_.dataSIZE - offset);

    samples.insert( samples.end(), &wave_[offset], &wave_[offset+real_size] );
}

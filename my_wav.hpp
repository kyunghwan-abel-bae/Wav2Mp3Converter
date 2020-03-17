#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>

#include <fstream>
#include <errno.h>
#include <cstring>

#ifdef WIN32
#define PATHSEP "\\"
#else
#define PATHSEP "/"
#endif

class MyWav {
public:
    MyWav (const char* file_path);

    bool is_valid_file() { return is_valid_file_; }

    int32_t get_samples_per_sec() const;
    int32_t get_avg_bytes_per_sec() const;
    int16_t get_channels() const;

    const char* get_file_name() const;

    double get_file_size(char unit=BYTE) const;

    void get_samples( unsigned int offset, unsigned int size, std::vector<unsigned char>& samples) const;

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

    enum {
        BYTE = 0,
        KILOBYTE = 1,
        MEGABYTE = 2,
        GIGABYTE = 3,
    };

public:
    static const int RIFF_SIZE      = 12;
    static const int FMTHDR_SIZE    = 8;
    static const int FMT_SIZE       = 16;
    static const int DATA_SIZE      = 8;
    static const int FACT_SIZE      = 8;

private:
    std::string extract_file_name_from_path(const char* file_path);
    std::string extract_file_name_from_path(std::string file_path);

    std::vector<char> wave_;

    RIFF                    riff_;
    FMTHDR                  fmthdr_;
    FMT                     fmt_;
    std::vector<char>       fmt_extra_bytes_;
    FACT                    fact_;
    DATA                    data_;
    int16_t                 extra_param_length_;
    std::vector<char>       extra_param_;

    bool is_valid_file_;
    unsigned long file_size_;
    std::string file_name_;
};

MyWav::MyWav (const char* file_path)
                                    : is_valid_file_(true)
{
    fmt_.wFormatTag = 0;
    extra_param_length_ = 0;
    fact_.samplesNumber = -1;

    std::ifstream file( file_path, std::ios_base::binary | std::ios_base::in );
    if( file.is_open() == false ) {
        is_valid_file_ = false;
        return;
    }

    // extract file name
    file_name_ = extract_file_name_from_path(file_path);

    file.read( reinterpret_cast<char*>( &riff_ ), RIFF_SIZE );
    file.read( reinterpret_cast<char*>( &fmthdr_ ), FMTHDR_SIZE );

    file.read( reinterpret_cast<char*>( &fmt_ ), FMT_SIZE );

    unsigned fmt_extra_bytes = fmthdr_.fmtSIZE - FMT_SIZE;

    char label_wav[4] = {'W', 'A', 'V', 'E'};

    if( !file ||
            !std::equal(std::begin(label_wav), std::end(label_wav), std::begin(riff_.riffFORMAT))) {
        is_valid_file_ = false;
        return;
    }

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

    file.seekg(0, std::ios::end);
    file_size_ = file.tellg();
}

std::string MyWav::extract_file_name_from_path(const char* file_path) {
    return extract_file_name_from_path(std::string(file_path));
}

std::string MyWav::extract_file_name_from_path(std::string file_path) {
    std::string path(file_path);
    std::string path_sep(PATHSEP);

    size_t index_path_sep = path.find_last_of(PATHSEP);

    return path.substr(index_path_sep + path_sep.length(), path.length());
}

const char* MyWav::get_file_name() const {
    return file_name_.c_str();
}

double MyWav::get_file_size(char unit) const {

    double divider = 1.0;

    for(int i=0;i<unit;i++) {
        divider = divider * 1000.0;
    }

    return static_cast<double>(file_size_/divider);
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

void MyWav::get_samples( unsigned int offset, unsigned int size, std::vector<unsigned char>& samples) const {
    if( offset > (unsigned)data_.dataSIZE )
        return;

    unsigned int real_size = ( offset + size ) < ( unsigned ) data_.dataSIZE ? size : ((unsigned) data_.dataSIZE - offset - 1);

    samples.insert( samples.end(), &wave_[offset], &wave_[offset+real_size] );
}

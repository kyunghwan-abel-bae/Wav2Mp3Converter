#include <errno.h>
#include <cstring>

#include "my_wav.hpp"

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

    unsigned int fmt_extra_bytes = fmthdr_.fmtSIZE - FMT_SIZE;

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

double MyWav::get_file_size(MyWav::UNIT unit) const {

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
    unsigned int data_size = static_cast<unsigned int>(data_.dataSIZE);
    if( offset > data_size )
        return;

    unsigned int real_size = ( offset + size ) < data_size ? size : (data_size - offset - 1);

    samples.insert( samples.end(), &wave_[offset], &wave_[offset+real_size] );
}

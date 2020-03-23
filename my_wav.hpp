#ifndef MYWAV_H
#define MYWAV_H

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <fstream>

#ifdef WIN32
#define PATHSEP "\\"
#else
#define PATHSEP "/"
#endif

class MyWav {
public:
    enum UNIT {
        BYTE = 0,
        KILOBYTE = 1,
        MEGABYTE = 2,
        GIGABYTE = 3,
    };

    MyWav (const char* file_path);

    bool is_valid_file() { return is_valid_file_; }

    int32_t get_samples_per_sec() const;
    int32_t get_avg_bytes_per_sec() const;
    int16_t get_channels() const;

    const char* get_file_name() const;

    double get_file_size(MyWav::UNIT unit=BYTE) const;

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



#endif // MYWAV_H

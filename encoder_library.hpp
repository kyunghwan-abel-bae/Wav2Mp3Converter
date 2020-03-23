#ifndef ENCODERLIBRARY_H
#define ENCODERLIBRARY_H

#define HAVE_STRUCT_TIMESPEC

#define PTW32_STATIC_LIB

#include <pthread.h>

#include "my_wav.hpp"

typedef struct {
    const char* file_path;
    std::string* file_name;
    double* src_size;
    long* des_size;
    int* result_status;
} RESULT_ARGS;

enum ENCODE_RESULT_STATUS {
    SUCCESS = 0,
    FAILED_NOT_VALID_FILE = -1
};

static void* EncodeMP3ByThread(void *thread_args);

class Encoder {
public:
    Encoder();

    virtual ~Encoder(){}

    virtual void EncodeTo(const char* file_type) = 0;

    void add_encoding_source_path(const std::string encoding_source_path);
    void add_encoding_source_path(const char* encoding_source_path);
    void set_encoding_source_path(const char* encoding_source_path);
    void set_num_threads(unsigned int num_threads);

protected:
    unsigned int num_threads_;
    const char* encoding_source_path_;
    std::vector<std::string> encoding_source_paths_;
};

class NullEncoder : public Encoder {
    ~NullEncoder() {}
    void EncodeTo(const char* file_type);
};

class LameWavEncoder : public Encoder {
public:
    ~LameWavEncoder() {}

    void EncodeTo(const char* file_type="");

private:

    void PrintOutEncodingResult();

    // er = encoding result
    unsigned int er_count_;
    unsigned int used_threads_count_;

    std::vector<std::string> vec_er_file_name_;
    std::vector<int> vec_er_status_;
    std::vector<double> vec_er_src_size_;
    std::vector<long> vec_er_des_size_;
};

class EncoderLibrary {
public:
    virtual ~EncoderLibrary(){}
    
    virtual Encoder* ReturnEncoderBySourceType(const char* file_type) = 0;
};

class LameEncoderLibrary : public EncoderLibrary {
public:
    ~LameEncoderLibrary(){}

    Encoder* ReturnEncoderBySourceType(const char* file_type="");
};

#endif // ENCODERLIBRARY_H

#define HAVE_STRUCT_TIMESPEC

#include <lame.h>
#include <pthread.h>

#include <iomanip>

#include "my_wav.hpp"

using namespace std;

typedef struct {
    const char* file_path;
    string* file_name;
    double* src_size;
    long* des_size;
    int* result_status;
} RESULT_ARGS;

enum THREAD_RESULT_STATUS {
    SUCCESS = 0,
    FAILED_NOT_VALID_FILE = -1
};

void* EncodeMP3ByThread(void *thread_args) {

    const int kWAV_SIZE = 8192;
    const int kMP3_SIZE = 8192;

    RESULT_ARGS* args = reinterpret_cast<RESULT_ARGS*>(thread_args);

    const char* source_path = args->file_path; //reinterpret_cast<const char*>(thread_args);

    string str_encoding_source_path(source_path);

    size_t index_dot = str_encoding_source_path.find_last_of(".");
    string str_encoding_destination_path = str_encoding_source_path.substr(0, index_dot) + ".mp3";

    MyWav wav(source_path);
    if(!wav.is_valid_file()) {
        *(args->file_name) = string(wav.get_file_name());
        *(args->result_status) = THREAD_RESULT_STATUS::FAILED_NOT_VALID_FILE;
        return nullptr;
    }

    std::ofstream mp3(str_encoding_destination_path, std::ios_base::binary|std::ios_base::out);

    int32_t sample_rate = wav.get_samples_per_sec();
    int32_t byte_rate = wav.get_avg_bytes_per_sec();
    int16_t channels = wav.get_channels();

    unsigned int offset = 0;
    unsigned int k = (channels == 1) ? 1 : 2;
    unsigned int size = kWAV_SIZE * k * sizeof(short int);

    std::vector<unsigned char> wav_buffer;
    wav_buffer.reserve( sizeof(short int) * kWAV_SIZE * k);
    unsigned char mp3_buffer[kMP3_SIZE];

    lame_t lame = lame_init();

    lame_set_in_samplerate(lame, sample_rate);
    lame_set_brate(lame, byte_rate);

    if(channels == 1) {
        lame_set_num_channels(lame, 1);
        lame_set_mode(lame, MONO);
    }
    else {
        lame_set_num_channels(lame, channels);
    }

    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);


    while( true ) {
        wav.get_samples( offset, size, wav_buffer );

        unsigned int read = wav_buffer.size();

        offset += read;

        if(read > 0) {
            int write = 0;

            if(channels == 1) {
                int read_shorts = read / 2;
                write = lame_encode_buffer( lame, reinterpret_cast<short int*>( &wav_buffer[0] ), nullptr, read_shorts, mp3_buffer, kMP3_SIZE );
            }
            else {
                int read_shorts = read / 4;
                write = lame_encode_buffer_interleaved( lame, reinterpret_cast<short int*>( &wav_buffer[0] ), read_shorts, mp3_buffer, kMP3_SIZE );
            }

            wav_buffer.clear();

            mp3.write( reinterpret_cast<char*>( mp3_buffer ) , write );
        }

        if(read < size) {
            int write = lame_encode_flush(lame, mp3_buffer, kMP3_SIZE);
            mp3.write( reinterpret_cast<char*>(mp3_buffer), write);

            break;
        }
    }

    *(args->result_status) = THREAD_RESULT_STATUS::SUCCESS;
    *(args->src_size) = wav.get_file_size();
    *(args->des_size) = mp3.tellp();
    *(args->file_name) = string(wav.get_file_name());

    lame_close(lame);

    return nullptr;
}

class Encoder {
public:
    Encoder() : num_threads_(1) {}

    virtual ~Encoder(){}

    virtual void EncodeTo(const char* file_type) = 0;

    void add_encoding_source_path(const string encoding_source_path) {
        encoding_source_paths_.push_back(encoding_source_path);
    }

    void add_encoding_source_path(const char* encoding_source_path) {
        add_encoding_source_path(string(encoding_source_path));
    }

    void set_encoding_source_path(const char* encoding_source_path) {
        encoding_source_path_ = encoding_source_path;
    }

    void set_num_threads(unsigned int num_threads) {
        num_threads_ = num_threads;
    }

protected:
    unsigned int num_threads_;
    const char* encoding_source_path_;
    vector<string> encoding_source_paths_;
};

class NullEncoder : public Encoder {
    ~NullEncoder() {}
    void EncodeTo(const char* file_type) {}
};

class LameWavEncoder : public Encoder {
public:
    ~LameWavEncoder() {}

    void EncodeTo(const char* file_type = "") {

        if(encoding_source_paths_.size() == 0) {
            cerr << "Target files not found" << endl;
            return ;
        }

        void* (*func_worker)(void*) = nullptr;

        if(strcmp(file_type, ".mp3") == 0) {
            func_worker = EncodeMP3ByThread;
        }
        // else if(strcmp(file_type, ".other_file_extension"))

        if(func_worker == nullptr)
            return;

        pthread_t *threads = new pthread_t[num_threads_];

        RESULT_ARGS *args = new RESULT_ARGS[num_threads_];

        /*
        vector<int> vec_result_status(encoding_source_paths_.size());
        vector<double> vec_src_size(encoding_source_paths_.size());
        vector<long> vec_des_size(encoding_source_paths_.size());
        vector<string> vec_file_name(encoding_source_paths_.size());
        */

        const vector<string> k_vec_base {"byte", "kB", "MB", "GB"};

        auto it = encoding_source_paths_.begin();
        unsigned int it_count = 0;

        bool is_done = false;

        cout << "==============================" << endl;
        cout << "ENCODING ..." << endl;
        while(true) {

            for(unsigned int i=0;i<num_threads_;i++) {
                if(it == encoding_source_paths_.end()) {
                    is_done = true;
                    num_threads_ = i;
                    break;
                }

                cout << (*it).c_str() << endl;

                args[i].file_path = (*it).c_str();
                args[i].result_status = &(vec_result_status[it_count]);
                args[i].src_size = &(vec_src_size[it_count]);
                args[i].des_size = &(vec_des_size[it_count]);
                args[i].file_name = &(vec_file_name[it_count]);

                pthread_create(&threads[i], nullptr, func_worker, const_cast<void*>(
                                                                    (reinterpret_cast<const void*>(&args[i]))
                                                                  ));

                it++;
                it_count++;
            }

            for(unsigned int i=0;i<num_threads_;i++) {
                pthread_join(threads[i], nullptr);
            }

            if(is_done)
                break;
        }


        cout << "==============================" << endl;
        cout << "RESULT" << endl << endl;

        cout << left;
        cout << " ";
        cout << setw(10) << "FILE NAME";
        cout << setw(2) << "|";
        cout << setw(8) << "STATUS";
        cout << setw(2) << "|";
        cout << setw(10) << "DESCRIPTION";
        cout << endl;


        for(unsigned int i=0;i<it_count;i++) {
            string str_file_name = vec_file_name[i];
            string str_result_status = vec_result_status[i] == THREAD_RESULT_STATUS::SUCCESS ? "SUCCESS" : "FAILED";
            string str_description;

            str_file_name = str_file_name.substr(0, str_file_name.find_last_of("."));

            if(str_file_name.length() > 10) {
                str_file_name = ".." + str_file_name.substr(str_file_name.length()-8, str_file_name.length()-1);
            }

            int result_status = vec_result_status[i];
            double encoded_file_size;

            string str_base = k_vec_base[0];
            double base = 1000.0;

            string str_compression_rate = "";
            double compression_rate = 0;

            switch(result_status) {
            case THREAD_RESULT_STATUS::SUCCESS :
                encoded_file_size = vec_des_size[i];

                for(unsigned int i=1;i<k_vec_base.size();i++) {
                    if(vec_des_size[i] < base)
                        break;

                    encoded_file_size /= base;

                    str_base = k_vec_base[i];
                    base *= 1000.0;
                }

                if(vec_src_size[i] > 0) {
                    compression_rate = vec_des_size[i]/vec_src_size[i] * 100;

                    str_compression_rate = "(" + to_string(compression_rate) + "%)";
                }

                str_description = to_string(encoded_file_size) + " " + str_base + str_compression_rate;

                break;

            case THREAD_RESULT_STATUS::FAILED_NOT_VALID_FILE :

                str_description = "Not Valid File";

                break;
            default :

                str_description = "Undefiend Error";

                break;
            }

            cout << setw(11) << str_file_name;
            cout << setw(2) << "|";
            cout << setw(8) << str_result_status;
            cout << setw(2) << "|";
            cout << setw(10) << str_description;
            cout << endl;
        }

        delete[] args;
        delete[] threads;
    }

private:
    vector<string> vec_file_name_;
    vector<int> vec_result_status_;
    vector<double> vec_src_size_;
    vector<long> vec_des_size_;
};

class EncoderLibrary {
public:
    virtual ~EncoderLibrary(){}
    
    virtual Encoder* ReturnEncoderBySourceType(const char* file_type) = 0;
};

class LameEncoderLibrary : public EncoderLibrary {
public:
    ~LameEncoderLibrary(){}

    Encoder* ReturnEncoderBySourceType(const char* file_type="") {

        if(strcmp(file_type, ".wav") == 0) {
            return new LameWavEncoder();
        }

        cerr << "Failed to find a proper encoder library" << endl;

        return new NullEncoder();
    }
};

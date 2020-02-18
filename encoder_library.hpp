#include <lame/lame.h>
#include <pthread.h>

#include "my_wav.hpp"

using namespace std;
    
typedef struct {
    //lame_global_flags*   lame;
    lame_t*              lame;
    //std::vector<unsigned char>*   wav_buffer;
    unsigned char*       wav_buffer;
    unsigned char*       mp3_buffer;
    unsigned int*        num_samples;
    unsigned int*        num_encoded_samples;
    unsigned int         mp3_buffer_size; 
} LAME_ENCODE_ARGS;

void* EncodeMP3ByThread(void *thread_args) {

    const int kWAV_SIZE = 8192;
    const int kMP3_SIZE = 8192;

    const char* source_path = reinterpret_cast<const char*>(thread_args);

    string str_encoding_source_path(source_path);

    size_t index_dot = str_encoding_source_path.find_last_of(".");
    string str_encoding_destination_path = str_encoding_source_path.substr(0, index_dot) + ".mp3";

    MyWav wav(source_path);

    std::ofstream mp3(str_encoding_destination_path, std::ios_base::binary|std::ios_base::out);

    unsigned int sample_rate = wav.get_samples_per_sec();
    unsigned int byte_rate = wav.get_avg_bytes_per_sec();
    unsigned int channels = wav.get_channels();

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
                // improved by KH
                unsigned read_shorts = read / 2;
                write = lame_encode_buffer( lame, reinterpret_cast<short int*>( &wav_buffer[0] ), NULL, read_shorts, mp3_buffer, kMP3_SIZE );
            }
            else {
                // improved by KH
                unsigned int read_shorts = read / 4;

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

    lame_close(lame);

    return NULL;
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

    void set_num_threads(int num_threads) {
        num_threads_ = num_threads;
    }

protected:
    int num_threads_;
    const char* encoding_source_path_;
    vector<string> encoding_source_paths_;
};

class LameWavEncoder : public Encoder {
public:
    ~LameWavEncoder() {}

    void SetLameEncodeOptions(lame_t* lame, unsigned int channels, unsigned int sample_rate, unsigned int byte_rate) {
        *lame = lame_init();

        lame_set_in_samplerate(*lame, sample_rate);
        lame_set_brate(*lame, byte_rate);

        if(channels == 1) {
            lame_set_num_channels(*lame, 1);
            lame_set_mode(*lame, MONO);
        }
        else {
            lame_set_num_channels(*lame, channels);
        }

        lame_set_VBR(*lame, vbr_default);
        lame_init_params(*lame);

    }

    void EncodeTo(const char* file_type = "") {

        void* (*func_worker)(void*) = NULL;

        if(strcmp(file_type, ".mp3") == 0) {
            func_worker = EncodeMP3ByThread;
        }
        else
            return;

        pthread_t *threads = new pthread_t[num_threads_];

        auto it = encoding_source_paths_.begin();
        bool is_done = false;
        while(true) {
            for(int i=0;i<num_threads_;i++) {
                if(it == encoding_source_paths_.end()) {
                    is_done = true;
                    num_threads_ = i;
                    break;
                }

                cout << "thread index : " << i << endl;

                pthread_create(&threads[i], NULL, func_worker, (void*)((*it).c_str()));

                ++it;
            }

            for(int i=0;i<num_threads_;i++) {
                pthread_join(threads[i], NULL);
            }

            if(is_done)
                break;
        }

        delete[] threads;
    }
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

        // checked by KH
        cout << "unsupported file type" << endl;
        // return NullEncoder()
        return NULL;
    }
};

#include <lame/lame.h>
#include <pthread.h>

#include "my_wav.hpp"
    
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

void *EncodeMP3ByThread(void *thread_args) {

    std::cout << "EncodeMP3ByThread" << std::endl;
    
    LAME_ENCODE_ARGS* arg = (LAME_ENCODE_ARGS*) thread_args;
   
    *(arg->num_encoded_samples) = lame_encode_buffer_interleaved(*(arg->lame), reinterpret_cast<short int*>(arg->wav_buffer), *(arg->num_samples), arg->mp3_buffer, arg->mp3_buffer_size);

    //*(arg->num_encoded_samples) = lame_encode_buffer_interleaved(lame, reinterpret_cast<short int*>(arg->wav_buffer), *(arg->num_samples), arg->mp3_buffer, arg->mp3_buffer_size);
    
    //*(arg->num_encoded_samples) = lame_encode_buffer_interleaved(*(arg->lame), reinterpret_cast<short int*>(&(*(arg->wav_buffer))[0]), *(arg->num_samples), arg->mp3_buffer, arg->mp3_buffer_size);
    

    return NULL;
}

class Encoder {
public:
    Encoder() : num_threads_(1) {}

    virtual ~Encoder(){};

    virtual void EncodeTo(const char* file_type) = 0;

    void set_encoding_source_path(const char* encoding_source_path) {
        encoding_source_path_ = encoding_source_path;
    }

    void set_num_threads(int num_threads) {
        num_threads_ = num_threads;
    }

protected:
    int num_threads_;
    const char* encoding_source_path_;

    enum {
        DOT_WAV_LENGTH = 4,
    };
};

class LameWavEncoder : public Encoder {
public:

    ~LameWavEncoder() {}

    enum { 
        WAV_SIZE = 8192,
        MP3_SIZE = 8192,
    };

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

        // MyWave wave(src)

        if(strcmp(file_type, ".mp3") == 0) {
            // ofstream mp3(des)
            
            EncodeToMp3(num_threads_);
            //else EncodeToMp3();
            //if(num_threads_ > 0) EncodeToMp3(num_threads_);
            //else EncodeToMp3();

            //else EncodeToMp3(wave, mp3);
        } else  {
            cout << "unsupported file type" << endl;
        }
        
    }

    void EncodeToMp3(int num_threads) {

        int source_length = strlen(encoding_source_path_);

        char encoding_destination_path[source_length] = {};

        strncpy(encoding_destination_path, encoding_source_path_, (source_length-Encoder::DOT_WAV_LENGTH));

        strcat(encoding_destination_path, ".mp3");

        MyWav wav(encoding_source_path_); 

        std::ofstream mp3(encoding_destination_path, std::ios_base::binary|std::ios_base::out);

        unsigned int sample_rate = wav.get_samples_per_sec();
        unsigned int byte_rate = wav.get_avg_bytes_per_sec();
        unsigned int channels = wav.get_channels();

        unsigned int offset = 0;
        unsigned int k = (channels == 1) ? 1: 2;
        unsigned int size = WAV_SIZE * k * sizeof(short int);

        pthread_t *threads = new pthread_t[num_threads];
        LAME_ENCODE_ARGS *thread_args = new LAME_ENCODE_ARGS[num_threads];

        lame_t lame[num_threads];

        std::vector<unsigned char> wav_buffer[num_threads];

        unsigned char mp3_buffer[num_threads][MP3_SIZE] = {};

        unsigned int num_samples[num_threads] = {};
        unsigned int num_encoded_samples[num_threads] = {};

        // test_point

        // Init thread_args
        for(int i=0;i<num_threads;i++) {
            SetLameEncodeOptions(&(lame[i]), channels, sample_rate, byte_rate);
            wav_buffer[i].reserve( sizeof(short int) * WAV_SIZE * k ); 

            // init
            thread_args[i].lame = &(lame[i]);
            thread_args[i].wav_buffer = &(wav_buffer[i][0]);
            thread_args[i].mp3_buffer = &(mp3_buffer[i][0]);
            thread_args[i].num_samples = &(num_samples[i]);
            thread_args[i].num_encoded_samples = &(num_encoded_samples[i]);
            thread_args[i].mp3_buffer_size = MP3_SIZE;
        }

        int for_test = 0;
        int test_count = 0;

        bool is_done = false;
        while( true ) {
           /* 
            if(++for_test == 3000)
                break;
           */ 
            unsigned int read[num_threads] = {};
            unsigned int write[num_threads] = {};

            // extract samples (in thread_args[i].wav_buffer)
            for(int i=0;i<num_threads;i++) {
                wav_buffer[i].clear();

                wav.get_samples( offset, size, wav_buffer[i] );
                read[i] = wav_buffer[i].size();
                
                offset += read[i];
            }                

            // create thread
            for (int i=0;i<num_threads;i++) {
                memset(mp3_buffer[i], 0, sizeof(mp3_buffer[i]));

                if(read[i] < size) {
                    num_threads = i;
                    is_done = true;
                    break;
                }

                if(channels == 1) {
                }
                else {
                    num_samples[i] = read[i] / 4; // 4 stands for sizeof(unsigned int)
                    pthread_create(&threads[i], NULL, EncodeMP3ByThread, (void*)(&thread_args[i]));
                }
            }

            // wait threads
            for (int i=0;i<num_threads;i++) {
                pthread_join( threads[i], NULL );
            }

            // writing
            for(int i=0;i<num_threads;i++) {
                mp3.write( reinterpret_cast<char*>(&mp3_buffer[i]), num_encoded_samples[i]);  
                cout << mp3.tellp() << endl;
            }
            cout << "end loop" << endl;


            // flush final part
            if(is_done) {
                unsigned char last_mp3buf[MP3_SIZE] = {};
                int num_encoded_samples = lame_encode_flush( lame[0], last_mp3buf, MP3_SIZE ); 

                mp3.write( reinterpret_cast<char*>(last_mp3buf) , num_encoded_samples );
                break;
            }
        }

        for(int i=0;i<num_threads;i++) {
            lame_close(lame[i]);
        }

        delete[] threads;
        delete[] thread_args;
    }

   
    void EncodeToMp3() {
        int source_length = strlen(encoding_source_path_);

        char encoding_destination_path[source_length] = {};

        strncpy(encoding_destination_path, encoding_source_path_, (source_length-Encoder::DOT_WAV_LENGTH));
        
        strcat(encoding_destination_path, ".mp3");

        MyWav wav(encoding_source_path_); 

        std::ofstream mp3(encoding_destination_path, std::ios_base::binary|std::ios_base::out);

        unsigned int sample_rate = wav.get_samples_per_sec();
        unsigned int byte_rate = wav.get_avg_bytes_per_sec();
        unsigned int channels = wav.get_channels();

        //const int WAV_SIZE = 8192;
        //const int MP3_SIZE = 8192;

        unsigned int offset = 0;
        unsigned int k = (channels == 1) ? 1: 2;
        unsigned int size = WAV_SIZE * k * sizeof(short int);

        std::vector<unsigned char> wav_buffer;
        wav_buffer.reserve( sizeof(short int) * WAV_SIZE * k );
        unsigned char mp3_buffer[MP3_SIZE];

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
                    write = lame_encode_buffer( lame, reinterpret_cast<short int*>( &wav_buffer[0] ), NULL, read_shorts, mp3_buffer, MP3_SIZE );
                }
                else {
                    // improved by KH
                    unsigned int read_shorts = read / 4;

                    write = lame_encode_buffer_interleaved( lame, reinterpret_cast<short int*>( &wav_buffer[0] ), read_shorts, mp3_buffer, MP3_SIZE );
                }

                wav_buffer.clear();

                mp3.write( reinterpret_cast<char*>( mp3_buffer ) , write );
            }



            if(read < size) {
                int write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
                mp3.write( reinterpret_cast<char*>(mp3_buffer), write);

                break;
            }
        }
        lame_close(lame);
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

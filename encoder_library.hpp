#include <lame/lame.h>

#include "my_wav.hpp"

class Encoder {
public:
    Encoder() : num_threads_(0) {}

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

    void EncodeTo(const char* file_type = "") {

        // MyWave wave(src)
        

        if(strcmp(file_type, ".mp3") == 0) {
            // ofstream mp3(des)
            
            EncodeToMp3();
            //else EncodeToMp3();
            //if(num_threads_ > 0) EncodeToMp3(num_threads_);
            //else EncodeToMp3();

            //else EncodeToMp3(wave, mp3);
        } else  {
            cout << "unsupported file type" << endl;
        }
        
    }

    void EncodeToMp3(int num_threads) {
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

        const int kWAV_SIZE = 8192;
        const int kMP3_SIZE = 8192;

        unsigned int offset = 0;
        unsigned int k = (channels == 1) ? 1: 2;
        unsigned int size = kWAV_SIZE * k * sizeof(short int);

        std::vector<char> wav_buffer;
        wav_buffer.reserve( sizeof(short int) * kWAV_SIZE * k );
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

#include <lame/lame.h>

#include "my_wave.h"

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
            
            if(num_threads_ > 0) EncodeToMp3(num_threads_);
            else EncodeToMp3();

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

        MyWave wav(encoding_source_path_); 

        std::ofstream mp3(encoding_destination_path, std::ios_base::binary|std::ios_base::out);

// ###################### HERE IS NEW FUNCTION #########





    }

    void EncodeToMp3() {
        // test by KH
        /*
        cout << "encode to mp3" << endl;
        cout << "file_soure : " << encoding_source_path_ << endl;
        */

        int source_length = strlen(encoding_source_path_);

        char encoding_destination_path[source_length] = {};

        strncpy(encoding_destination_path, encoding_source_path_, (source_length-Encoder::DOT_WAV_LENGTH));
        
        strcat(encoding_destination_path, ".mp3");

        // Wave file analysis using MyWave
        // if(num_threads_ > 0) 
        //      encoding_mp3_with_pthread(&mywave)

        MyWave wav(encoding_source_path_); 

        std::ofstream mp3(encoding_destination_path, std::ios_base::binary|std::ios_base::out);

        //// ############# HERE IS NEW FUNCTION ##############
        /*
        unsigned sample_rate = wav.get_samples_per_sec();
        unsigned byte_rate = wav.get_avg_bytes_per_sec();
        unsigned channels = wav.get_channels();

        const int WAV_SIZE = 8192;
        const int MP3_SIZE = 8192;

        unsigned offset = 0;

        std::vector<char> wav_buffer;
        wav_buffer.reserve( sizeof(short int) * WAV_SIZE * 2);
        unsigned mp3_buffer[MP3_SIZE];

        lame_t lame = lame_init();
        // 		lame_global_flags *gfp = lame_init();

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
            int k = (channels == 1) ? 1 : 2;
            unsigned size = WAV_SIZE * k * sizeof( short int );
            wav.get_samples(offset, size, wav_buffer);
            unsigned read = wav_buffer.size();

            offset += read;

            if(read > 0) {
                int write = 0;

                if(channels == 1) {
                    // improved by KH
                    unsigned read_shorts = read / 2;

                    //write = lame_encode_buffer(lame, reinterpret_cast<short int*>(&wav_buffer[0]), NULL, read_shorts, mp3_buffer, MP3_SIZE);
                    write = lame_encode_buffer( lame, reinterpret_cast<short int*>(&wav_buffer[0]), NULL, read_shorts, mp3_buffer, MP3_SIZE );
                }
                else {
                    // improved by KH
                    unsigned read_shorts = read / 4;

                    write = lame_encode_buffer_interleaved( lame, reinterpret_cast<short int*>(&wav_buffer[0]), read_shorts, mp3_buffer, MP3_SIZE );
                }

                wav_buffer.clear();
                mp3.write( reinterpret_cast<char*>(mp3_buffer), write);
            }

            if(read < size) {
                int write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
                mp3.write( reinterpret_cast<char*>(mp3_buffer), write);

                break;
            }
        }

        lame_close(lame);
        */

        /*
        do {
            read = fread(pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
            if (read == 0)
                write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
            else
                write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
            fwrite(mp3_buffer, write, 1, mp3);
        } while (read != 0);
        */


















        

        // test by KH
        //cout << "file_destination : " << encoding_destination_path << endl;
/*
        int read, write;

        FILE *wav = fopen(encoding_source_path_, "rb");
        FILE *mp3 = fopen(encoding_destination_path, "wb");

        const int WAV_SIZE = 8192;
        const int MP3_SIZE = 8192;

        short int wav_buffer[WAV_SIZE*2];
        unsigned char mp3_buffer[MP3_SIZE];

        lame_t lame = lame_init();
        lame_set_in_samplerate(lame, 44100);
        lame_set_VBR(lame, vbr_default);
        lame_init_params(lame);

        do {
            read = fread(wav_buffer, 2*sizeof(short int), WAV_SIZE, wav);
            if (read == 0)
                write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
            else
                write = lame_encode_buffer_interleaved(lame, wav_buffer, read, mp3_buffer, MP3_SIZE);

            fwrite(mp3_buffer, write, 1, mp3);
        } while (read != 0);

        lame_close(lame);
        fclose(mp3);
        fclose(wav);

        */
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

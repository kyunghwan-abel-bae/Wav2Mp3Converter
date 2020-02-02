// let's go with step-by-step

#include <iostream>

#include "file_explorer.hpp"


// TEST
#include <stdio.h>
#include <lame/lame.h>

using namespace std;

// TEST

class Encoder {
public:
    virtual ~Encoder(){};

    virtual void EncodeTo(const char* file_type) = 0;

    void set_encoding_source_path(const char* encoding_source_path) {
        encoding_source_path_ = encoding_source_path;
    }

protected:
    const char* encoding_source_path_;

    enum {
        DOT_WAV_LENGTH = 4,
    };
};

class LameWavEncoder : public Encoder {
public:
    ~LameWavEncoder() {}

    void EncodeTo(const char* file_type = "") {

        if(strcmp(file_type, ".mp3") == 0)
            EncodeToMp3();
        else 
            cout << "unsupported file type" << endl;
        
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
        
        // test by KH
        //cout << "file_destination : " << encoding_destination_path << endl;

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

// TEST

int main(int _argc, char *_argv[]) {

    if(_argc != 2) {
        cout << "Usage : " << endl;
        return 0;
    }

    const char* path = _argv[1];

    FileExplorer explorer(path);

    if(!explorer.ExploreWithFileType(".wav")) {
        cout << "Failed to find wav files at the " << path << endl;
        return 0;
    }
    
    vector<string> vec_found_files_paths = explorer.vec_found_files_paths();

    EncoderLibrary *encoder_library = new LameEncoderLibrary();
    Encoder *encoder = encoder_library->ReturnEncoderBySourceType(".wav");

    for (auto it=vec_found_files_paths.begin();it!=vec_found_files_paths.end();++it) {
        encoder->set_encoding_source_path((*it).c_str());
        encoder->EncodeTo(".mp3");
    }

    //delete encoder;
    //delete encoder_library;

    return 0;
}



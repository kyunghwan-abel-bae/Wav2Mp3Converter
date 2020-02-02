#include "files_manager.h"

main(argc) {
    FileFinder finder = new FileFinder("wav");
    const char** const list = finder.find(argc);

    FileLoader loader = new FileLoader();
    FileSaver saver; 
        
    //Encoder encoder = new LameEncoder();

    EncoderLibrary encoder_library = new LameEncoderLibrary();
    Encoder encoder = encoder_libary->ReturnEncoderBySourceType(".wav");// encoder = new LameWavEncoder();
    
    /*
    encoder.wav_size(8192);
    encoder.mp3_size(8192);
    encoder.sample_rate(44100);
    */

    for (vector<string>::iterator it=vec_found_files_paths.begin();it!=vec_found_files_paths.end();++it) {
        encoder.set_encoding_source_path((*it).c_str());
        encoder.EncodeTo("mp3");
    }
}


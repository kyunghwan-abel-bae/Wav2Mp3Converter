#include <iostream>
#include <thread>

#include "file_explorer.hpp"
#include "encoder_library.hpp"

using namespace std;

int main(int _argc, char *_argv[]) {

    if(_argc != 2) {
        cout << "Usage : " << endl;
        return 0;
    }

    const char* path = _argv[1];

    FileExplorer explorer(path);

    if(!explorer.ExploreWithFileType(".wav")) {
        cerr << "Failed to find wav files at the " << path << endl;
        return 0;
    }
    
    vector<string> vec_found_files_paths = explorer.vec_found_files_paths();

    EncoderLibrary *encoder_library = new LameEncoderLibrary();
    Encoder *encoder = encoder_library->ReturnEncoderBySourceType(".wav");

    unsigned int num_threads = thread::hardware_concurrency();

    encoder->set_num_threads(1);
    for (auto it=vec_found_files_paths.begin();it!=vec_found_files_paths.end();++it) {
        encoder->add_encoding_source_path(*it);
    }

    encoder->EncodeTo(".mp3");

    delete encoder_library;

    return 0;
}



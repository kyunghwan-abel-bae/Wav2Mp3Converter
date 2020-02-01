// let's go with step-by-step

#include <iostream>

#include "file_explorer.hpp"

using namespace std;

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

    // test
    for (vector<string>::iterator it=vec_found_files_paths.begin();it!=vec_found_files_paths.end();++it) {
        cout << "!";
        cout << ' ' << *it;
        cout << endl;
    }

    return 0;
}



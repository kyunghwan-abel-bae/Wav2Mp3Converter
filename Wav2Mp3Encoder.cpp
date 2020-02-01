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
    vector<string> vec_found_file_paths;
    if(explorer.ExplorerWithFileType(".wav")) {
        
    }
    else 
        cout << "Failed to find wav files" << endl;


    /*
    vector<string> vec_str_files = list_dir(path, ".wav");

    for (vector<string>::iterator it=vec_str_files.begin();it!=vec_str_files.end();++it) {
        cout << "!";
        cout << ' ' << *it;
        cout << endl;
    }

    */

    //vector<char*> file_list = list_dir(path, ".wav");
    //file_list.length == 0 No wave files found

    return 0;
}



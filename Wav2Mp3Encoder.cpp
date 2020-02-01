// let's go with step-by-step

#include <iostream>

#include <vector>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

using namespace std;

// checked by KH
//static void list_dir(const char* kPath="")
static vector<string> list_dir(const char* kPath="", const char* kFileType="")
{
    struct dirent *entry;
    vector<string> vec_files;

    DIR *dir = opendir(kPath);
    if (dir == NULL) {
        return vec_files;
    }
    
    if( (strlen(kPath) == 0) || strlen(kFileType) == 0 ) {
        return vec_files;
    }

    char arr_file_path[100] = {0};
    strcat(arr_file_path, kPath); 
    // checked by KH
    //if( (path[strlen([path]-1)] != PATH_DIVIDER )
    if(kPath[strlen(kPath)-1] != '/')
        strcat(arr_file_path, "/");
    
    while ((entry = readdir(dir)) != NULL) {
        char* file_name = entry->d_name;
        char* extracted_file_type = strrchr(file_name, '.');

        if(extracted_file_type != NULL) {
            if(strcmp(extracted_file_type, kFileType) == 0)  {
                char ch_path[100] = {0};

                strcat(ch_path, arr_file_path);
                strcat(ch_path, file_name);
                
                string str_file = string(ch_path);

                vec_files.push_back(str_file);
            }
        }
    }

    closedir(dir);

    return vec_files;
}

int main(int _argc, char *_argv[]) {

    if(_argc != 2) {
        cout << "Usage : " << endl;
        return 0;
    }

    const char* path = _argv[1];

    vector<string> vec_str_files = list_dir(path, ".wav");

    for (vector<string>::iterator it=vec_str_files.begin();it!=vec_str_files.end();++it) {
        cout << "!";
        cout << ' ' << *it;
        cout << endl;
    }

    //vector<char*> file_list = list_dir(path, ".wav");
    //file_list.length == 0 No wave files found

    return 0;
}



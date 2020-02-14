#include <vector>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

using namespace std;

class FileExplorerOption {
public:
    enum { 
        FILE_PATH_LENGTH = 100,
    };
};

class FileExplorer : public FileExplorerOption {
public:
    FileExplorer(const char path[])
        : path_(path) {}

    const char* get_current_path() { return path_; }
    void set_current_path(const char path[]) {
        path_ = path;
    }

    vector<string> vec_found_files_paths() { return vec_found_files_paths_; }
    
    bool ExploreWithFileType(const char file_type[]) {

        if( (strlen(path_) == 0) || strlen(file_type) == 0 ) 
            return false;
        
        DIR *dir = opendir(path_);
        if (dir == NULL)
            return false;

        char dir_path[FileExplorerOption::FILE_PATH_LENGTH] = {0};
        strcat(dir_path, path_);
        // checked by KH
        //if(dir_path[strlen(dir_path)-1] != PATH_DIVIDER)
        if(dir_path[strlen(dir_path)-1] != '/')
            strcat(dir_path, "/");

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {

            const char* file_name = entry->d_name;
            const char* extracted_file_type = strrchr(file_name, '.');

            if(extracted_file_type != NULL) {
                if(strcmp(extracted_file_type, file_type) == 0) {
                    char file_path[FileExplorer::FILE_PATH_LENGTH] = {0};
                    strcat(file_path, dir_path);
                    strcat(file_path, file_name);

                    string str_file_path = string(file_path);
                    vec_found_files_paths_.push_back(str_file_path);
                }
            }
        }

        closedir(dir);

        return vec_found_files_paths_.size() > 0 ? true : false;
    }

private:
    vector<string> vec_found_files_paths_;
    const char* path_;
    int count_;
};

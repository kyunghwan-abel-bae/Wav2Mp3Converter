#include <vector>
#include <string>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>

using namespace std;

#ifdef WIN32
#define PATHSEP '\\'
#else
#define PATHSEP '/'
#endif

class FileExplorerOption {
public:
    enum { 
        FILE_PATH_LENGTH = 255,
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
        if (dir == nullptr)
            return false;

        char dir_path[FileExplorerOption::FILE_PATH_LENGTH] = {0};
        strcat(dir_path, path_);

        const size_t length_dir_path = strlen(dir_path);
        if(length_dir_path > FileExplorerOption::FILE_PATH_LENGTH)
            return false;

        char path_sep[1] = {PATHSEP};
        if(dir_path[length_dir_path-1] != path_sep[0])
            strcat(dir_path, path_sep);

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {

            const char* file_name = entry->d_name;
            const char* extracted_file_type = strrchr(file_name, '.');

            if(extracted_file_type != nullptr) {
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
};

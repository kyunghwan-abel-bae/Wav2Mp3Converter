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
    FileExplorer(const char kPath[])
        : kPath_(kPath) {}

    const char* get_current_path() { return kPath_; }
    void set_current_path(const char kPath[]) {
        kPath_ = kPath;
    }

    vector<string> vec_found_file_path() { return vec_found_file_path_; }
    
    bool ExploreWithFileType(const char kFileType[]) {

        if( (strlen(kPath_) == 0) || strlen(kFileType) == 0 ) 
            return false;
        
        DIR *dir = opendir(kPath_);
        if (dir == NULL)
            return false;

        char dir_path[FileExplorerOption::FILE_PATH_LENGTH] = {0};
        strcat(dir_path, kPath_);
        // checked by KH
        //if(dir_path[strlen(dir_path)-1] != PATH_DIVIDER)
        if(dir_path[strlen(dir_path)-1] != '/')
            strcat(dir_path, "/");

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {

            const char* kFileName = entry->d_name;
            const char* kExtractedFileType = strrchr(kFileName, '.');

            if(kExtractedFileType != NULL) {
                if(strcmp(kExtractedFileType, kFileType) == 0) {
                    char file_path[FileExplorer::FILE_PATH_LENGTH] = {0};
                    strcat(file_path, dir_path);
                    strcat(file_path, kFileName);

                    string str_file_path = string(file_path);
                    vec_found_file_path_.push_back(str_file_path);
                }
            }
        }

        closedir(dir);

        return true;
    }

private:
    vector<string> vec_found_file_path_;
    const char* kPath_;
    int count_;
};

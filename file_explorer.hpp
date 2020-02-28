#include <vector>
#include <string>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>

#ifdef WIN32
#define PATHSEP "\\"
#else
#define PATHSEP "/"
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

    std::vector<std::string> vec_found_files_paths() { return vec_found_files_paths_; }

    bool ExploreWithFileType(const char file_type[]) {

        if( (strlen(path_) == 0) || strlen(file_type) == 0 )
            return false;

        DIR *dir = opendir(path_);
        if (dir == nullptr)
            return false;

        char dir_path[FileExplorerOption::FILE_PATH_LENGTH] = {0};
        strcat_s(dir_path, FileExplorerOption::FILE_PATH_LENGTH, path_);

        const size_t length_dir_path = strlen(dir_path);
        if(length_dir_path > FileExplorerOption::FILE_PATH_LENGTH)
            return false;

        std::string path(dir_path);
        std::string path_sep(PATHSEP);

        size_t index_path_sep = path.find_last_of(PATHSEP);
        if(index_path_sep != (path.length()-path_sep.length())) {
            path += path_sep;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            const char* file_name = entry->d_name;
            const char* extracted_file_type = strrchr(file_name, '.');

            if(extracted_file_type != nullptr) {
                if(strcmp(extracted_file_type, file_type) == 0) {
                    std::string str_file_path = path + std::string(file_name);

                    vec_found_files_paths_.push_back(str_file_path);
                }
            }
        }

        closedir(dir);

        return vec_found_files_paths_.size() > 0 ? true : false;
    }

private:
    std::vector<std::string> vec_found_files_paths_;
    const char* path_;
};

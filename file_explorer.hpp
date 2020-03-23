#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <vector>
#include <string>

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
    FileExplorer();
    FileExplorer(const char path[]);

    const char* get_current_path();
    void set_current_path(const char path[]);

    std::vector<std::string> vec_found_files_paths();

    bool ExploreWithFileType(const char file_type[]);

private:
    std::vector<std::string> vec_found_files_paths_;
    const char* path_;
};

#endif // FILEEXPLORER_H

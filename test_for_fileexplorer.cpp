#include <iostream>

#include "file_explorer.hpp"

using namespace std;

int main() {
    FileExplorer fe("/home/abel/Desktop/tmp");

    //if(fe.ExplorerWithFileType(".wav")) {
    //
    if(fe.ExploreWithFileType(".wav")) {
      vector<string> vec_str_files = fe.vec_found_file_path();

      for (vector<string>::iterator it=vec_str_files.begin();it!=vec_str_files.end();++it) {
          cout << "!";
          cout << ' ' << *it;
          cout << endl;
      }
    }
    else {
        cout << "files not found" << endl;
    }

    return 0;
}

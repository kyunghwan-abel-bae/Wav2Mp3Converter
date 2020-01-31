#include "files_manager.h"

main(argc) {
  
    EncodingOut encoding_out = new LameEncodingOut();
    encodingOut.set_attributes(
            44100, vbr_default, "mp3"
    );
  
    FileFinder finder = new FileFinder("wav");
    const char** list[30] = finder.find(argc);

    FileLoader loader = new FileLoader();
    FileSaver saver; 
    for(loader->next()) {
        FileOut delegate = new FileOutImpl;
    //    FileOut file_out = new Byte2MP3Out(encoding_out(delegate));
        FileOut file_out = new Byte2MP3Out(encoding_out(new Wav2ByteOut(delegate)));
        file_out.write(list[i]); 
    }
}


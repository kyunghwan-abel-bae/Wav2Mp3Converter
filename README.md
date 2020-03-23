
# How to run "Wav2Mp3Converter"

<pre>
<code>
Usage: [PATH-TO-Wav2Mp3Converter]/Wav2Mp3Conveter [WAV FILES' LOCATION]

EX) $ /home/user/Desktop/Wav2Mp3Conveter /home/user/sound
</code>
</pre>

## Windows

- EXE Download link : [CLICK HERE](https://github.com/kyunghwan-abel-bae/Wav2Mp3Converter/releases/download/1.1/Wav2Mp3Converter.exe)
  - This is a static version, it works without pthread dynamic libs

## Linux

- To run, the make process is required
- Install the package follwing the build environment below

* * *

# Build environment

## Windows

- OS : Windows 10
- Tools(32 bit) : Qt(5.12.0), Lame(3.100), pthread(pthreads-w32-2-9-1-release), MSVC2017(Community), Python(2.7)
- If you failed to build the project, then you should make a proper build environment based on below "Steps to build".
- Steps to build
  - Install Python 2.7, and Add paths(C:\Python27, C:\Python27\Scripts) to Path variable in System variables
  - Install Visual Studio 2017 Community(including Visual C++ ATL&MFC for x86)
  - Open the Visual Studio 2017 Developer Command Prompt(Community)
  - Static qt configure : configure.bat -static -debug-and-release -prefix C:\Qt\static\5.12.0 -platform win32-msvc -opensource -confirm-license -nomake examples -nomake tests -static-runtime -opengl desktop -force-debug-info
  - nmake & nmake install
  - In the pthreads.2 folder, nmake clean VC-static & nmake install
  - Generate static lame lib from vc_solution
  - Download dirent from https://github.com/tronkko/dirent
  - In the Wav2Mp3Converter.pro, set paths about lame, pthread, dirent and msvc libs(msvcrt.lib, msvcmrt.lib)

## Linux

- OS : Ubuntu 18.04
- Tools : make, g++(7.5.0), Lame(3.100), pthread(already installed with Ubuntu 18.04, it is part of the GNU C Library)
- A Makefile is included, run make to build
  - g++ *.cpp -o Wav2Mp3Converter -lpthread -lmp3lame

# Challenge logs

All notable logs show how I solved many issues in this project

## 2020-3-17

- Fix the memory vulnerability which is detected by valgrind

## 2020-3-8

- Success to deploy windows application
- App runs on the Windows 7 without problems

## 2020-3-7

- Add paths about lame's static lib and include in .pro file (version : lame-3.100 from https://lame.sourceforge.io/download.php)
  - Solve the error about "__imp__strncpy" at the compile by linking the msvcrt.lib and msvcmrt.lib in MSVC
- Add paths about pthread's static lib and include in .pro file (version : pthreads-w32-2-9-1-release from https://sourceware.org/pthreads-win32)
  - A lib file of the prebuilt version is not for static compile, A lib file should be generated from the pthread source
- To get information of files and directories under Linux/UNIX style, download dirent.h from https://github.com/tronkko/dirent, and add a path in .pro file
- For the static compile, put below codes where the pthread is created and deleted
  - BOOL pthread_win32_process_attach_np (void);
  - BOOL pthread_win32_process_detach_np (void);
  - BOOL pthread_win32_thread_attach_np (void);
  - BOOL pthread_win32_thread_detach_np (void);

## 2020-3-1

- Set C++ static-build environments for Windows using Qt
- Build based on qt-everywhere-src-5.12.0, not existed version

## 2020-2-29

- Calcuate a compression rate
- Print the encoding process and result

## 2020-2-23

- Prepare and build Windows lame libraries through the Visual Studio Solution file in the 'lame-3.100' which can be found at the official website
- Solve a pthread error about <'timespec': 'struct' type redefinition> by specifying #define HAVE_STRUCT_TIMESPEC

## 2020-2-22

- Implement the filter for dummy wav files using riff fields
- Setting multiplatform variables such as win32, __linux__, etc

## 2020-2-18 *

- Switch from the sample-based encoding to the file-based encoding
- Encoding works with the file-based encoding in the multi-thread environment

## 2020-2-16 *

- Through debug process, there are thread safe issues in lame encode functions. A pthread process affected the other thread processes

## 2020-2-13

- Using Qt, I imported project in OS X for efficient debugging (Debug issue is about lame functions in multi-thread encoding)

## 2020-2-11

- The structure for multithread processes(sample-based) is prepared
- Encoding works with single thread in the multithread design

## 2020-2-9

- Improve struct variables with pointers
- Understanding for the optimal number of threads with below links
  - https://stackoverflow.com/questions/1718465/optimal-number-of-threads-per-core
  - https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
  - https://doitnow-man.tistory.com/16
  - optimal number : 1 thread per 1 cpu core (The sample-base threading proccess require only calculation, so 1 thread per core will normally achieve the best performance)

## 2020-2-8

- Implement the function which extracts samples from the wav file
- Declare struct variables for pthread processes
- Adapt the encoding processes to the threads

## 2020-2-6

- Extract functions for analysis of wav files to the my_wav.hpp
- Success to encode wav files to mp3 file without pthread

## 2020-2-4

- Analyze extracted wav files in c++
- Found a link to have a same purpose with this task
  - https://github.com/dheller1/lame_pthreads
  - However, threads of this project are working on the file-base, not the sample-base.

## 2020-2-3

- Understanding for encoding arguments concepts
  - Bit rate, Sample rate
  - Encoding options: ABR, VBR
  - Recommended encoder settings : https://wiki.hydrogenaud.io/index.php?title=Recommended_LAME#Recommended_encoder_settings


## 2020-2-2

- Found a good link to implement lame functions
  - https://stackoverflow.com/questions/2495420/is-there-any-lame-c-wrapper-simplifier-working-on-linux-mac-and-win-from-pure
- As an overall system design, I choose an Abstract factory design pattern instead of the decorator pattern

## 2020-2-1

- Extract functions of Filter&Load wav files to the file_manager.hpp
- Give up the decorator patern design because the project doen't have enough functions to implement the decorator pattern


## 2020-1-31

- Filter wav files
- First compile with compile options( -lpthread -lmp3lame )
- Design an overall structure using a Decorator pattern
- Understanding the pthread usages through below links
  - https://www.joinc.co.kr/w/Site/Thread/Beginning/PthreadApiReference
  - https://plming.tistory.com/62
  - https://wiserloner.tistory.com/292

## 2020-1-28

- Understanding the given coding test
- Development environment(ubuntu, lame, ...) is prepared

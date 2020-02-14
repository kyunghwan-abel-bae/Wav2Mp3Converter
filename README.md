# Challenge logs

All notable logs show how I solved many issues in this project


## 2020-2-11

- The structure for multithread processes(sample-base) is prepared
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

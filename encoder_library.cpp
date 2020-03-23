#include <lame/lame.h>
#include <iomanip>
#include <cstring>

#include "encoder_library.hpp"

void* EncodeMP3ByThread(void *thread_args) {

    const int kWAV_SIZE = 8192;
    const int kMP3_SIZE = 8192;

    RESULT_ARGS* args = reinterpret_cast<RESULT_ARGS*>(thread_args);

    const char* source_path = args->file_path;

    std::string str_encoding_source_path(source_path);

    size_t index_dot = str_encoding_source_path.find_last_of(".");
    std::string str_encoding_destination_path = str_encoding_source_path.substr(0, index_dot) + ".mp3";

    MyWav wav(source_path);

    if(!wav.is_valid_file()) {
        *(args->result_status) = ENCODE_RESULT_STATUS::FAILED_NOT_VALID_FILE;
        *(args->src_size) = 0;
        *(args->des_size) = 0;
        *(args->file_name) = std::string(wav.get_file_name());

        return nullptr;
    }

    std::ofstream mp3(str_encoding_destination_path, std::ios_base::binary|std::ios_base::out);

    int32_t sample_rate = wav.get_samples_per_sec();
    int32_t byte_rate = wav.get_avg_bytes_per_sec();
    int16_t channels = wav.get_channels();

    unsigned int offset = 0;
    unsigned int k = (channels == 1) ? 1 : 2;
    unsigned int size = kWAV_SIZE * k * sizeof(short int);

    std::vector<unsigned char> wav_buffer;
    wav_buffer.reserve( sizeof(short int) * kWAV_SIZE * k);
    unsigned char mp3_buffer[kMP3_SIZE];

    lame_t lame = lame_init();

    lame_set_in_samplerate(lame, sample_rate);
    lame_set_brate(lame, byte_rate);

    if(channels == 1) {
        lame_set_num_channels(lame, 1);
        lame_set_mode(lame, MONO);
    }
    else {
        lame_set_num_channels(lame, channels);
    }

    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);

    while( true ) {
        wav.get_samples( offset, size, wav_buffer );

        unsigned int read = wav_buffer.size();

        offset += read;

        if(read > 0) {
            int write = 0;

            std::cout << "channels : " << channels << std::endl;

            if(channels == 1) {
                int read_shorts = read / 2;
                write = lame_encode_buffer( lame, reinterpret_cast<short int*>( &wav_buffer[0] ), nullptr, read_shorts, mp3_buffer, kMP3_SIZE );
            }
            else {
                int read_shorts = read / 4;
                write = lame_encode_buffer_interleaved( lame, reinterpret_cast<short int*>( &wav_buffer[0] ), read_shorts, mp3_buffer, kMP3_SIZE );
            }

            wav_buffer.clear();

            mp3.write( reinterpret_cast<char*>( mp3_buffer ) , write );
        }

        if(read < size) {
            int write = lame_encode_flush(lame, mp3_buffer, kMP3_SIZE);
            mp3.write( reinterpret_cast<char*>(mp3_buffer), write);

            break;
        }
    }

    *(args->result_status) = ENCODE_RESULT_STATUS::SUCCESS;
    *(args->src_size) = wav.get_file_size();
    *(args->des_size) = mp3.tellp();
    *(args->file_name) = std::string(wav.get_file_name());

    lame_close(lame);

    return nullptr;
}

Encoder::Encoder()
    : num_threads_(1)
{}

void Encoder::add_encoding_source_path(const std::string encoding_source_path) {
    encoding_source_paths_.push_back(encoding_source_path);
}

void Encoder::add_encoding_source_path(const char* encoding_source_path) {
    add_encoding_source_path(std::string(encoding_source_path));
}

void Encoder::set_encoding_source_path(const char* encoding_source_path) {
    encoding_source_path_ = encoding_source_path;
}

void Encoder::set_num_threads(unsigned int num_threads) {
    num_threads_ = num_threads;
}

void NullEncoder::EncodeTo(const char* file_type)
{}

void LameWavEncoder::EncodeTo(const char* file_type) {

    if(encoding_source_paths_.size() == 0) {
        std::cerr << "Target files not found" << std::endl;
        return ;
    }

    void* (*func_worker)(void*) = nullptr;

    if(strcmp(file_type, ".mp3") == 0) {
        func_worker = EncodeMP3ByThread;
    }
    // else if(strcmp(file_type, ".other_file_extension"))

    if(func_worker == nullptr)
        return;

#ifdef WIN32
    pthread_win32_process_attach_np();
    pthread_win32_thread_attach_np();
#endif

    pthread_t *threads = new pthread_t[num_threads_];

    RESULT_ARGS *args = new RESULT_ARGS[num_threads_];

    vec_er_status_.resize(encoding_source_paths_.size());
    vec_er_src_size_.resize(encoding_source_paths_.size());
    vec_er_des_size_.resize(encoding_source_paths_.size());
    vec_er_file_name_.resize(encoding_source_paths_.size());

    unsigned int max_threads_count = num_threads_;

    auto it = encoding_source_paths_.begin();
    er_count_= 0;

    bool is_done = false;

    std::cout << "============================================================" << std::endl;
    std::cout << "ENCODING ..." << std::endl << std::endl;
    while(true) {

        for(unsigned int i=0;i<num_threads_;i++) {
            if(it == encoding_source_paths_.end()) {
                is_done = true;
                num_threads_ = i;
                break;
            }

            std::cout << (*it).c_str() << std::endl;

            args[i].file_path = (*it).c_str();
            args[i].result_status = &(vec_er_status_[er_count_]);
            args[i].src_size = &(vec_er_src_size_[er_count_]);
            args[i].des_size = &(vec_er_des_size_[er_count_]);
            args[i].file_name = &(vec_er_file_name_[er_count_]);

            pthread_create(&threads[i], nullptr, func_worker, const_cast<void*>(
                                                                (reinterpret_cast<const void*>(&args[i]))
                                                              ));

            it++;
            er_count_++;
        }

        for(unsigned int i=0;i<num_threads_;i++) {
            pthread_join(threads[i], nullptr);
        }

        if(is_done)
            break;
    }

    used_threads_count_ = er_count_ < max_threads_count ? er_count_ : max_threads_count;

    std::cout << std::endl << "DONE!" << std::endl;

    std::cout << "============================================================" << std::endl;

    PrintOutEncodingResult();

    delete[] args;
    delete[] threads;

#ifdef WIN32
    pthread_win32_thread_detach_np();
    pthread_win32_process_detach_np();
#endif
}

void LameWavEncoder::PrintOutEncodingResult() {

    std::cout << "RESULT" << std::endl << std::endl;

    std::cout << std::left;
    std::cout << " ";
    std::cout << std::setw(10) << "FILE NAME";
    std::cout << std::setw(2) << "|";
    std::cout << std::setw(8) << "STATUS";
    std::cout << std::setw(2) << "|";
    std::cout << std::setw(10) << "DESCRIPTION";
    std::cout << std::endl;

    const std::vector<std::string> k_vec_base {"byte", "kB", "MB", "GB"};

    for(unsigned int i=0;i<er_count_;i++) {
        std::string str_file_name = vec_er_file_name_[i];
        std::string str_result_status = vec_er_status_[i] == ENCODE_RESULT_STATUS::SUCCESS ? "SUCCESS" : "FAILED";
        std::string str_description;

        str_file_name = str_file_name.substr(0, str_file_name.find_last_of("."));

        if(str_file_name.length() > 10) {
            str_file_name = ".." + str_file_name.substr(str_file_name.length()-8, str_file_name.length()-1);
        }

        int result_status = vec_er_status_[i];
        double encoded_file_size;

        std::string str_base = k_vec_base[0];
        double base = 1000.0;

        std::string str_compression_rate = "";
        double compression_rate = 0;

        switch(result_status) {
        case ENCODE_RESULT_STATUS::SUCCESS :
            encoded_file_size = vec_er_des_size_[i];

            for(unsigned int j=1;j<k_vec_base.size();j++) {
                if(vec_er_des_size_[i] < base)
                    break;

                encoded_file_size /= base;

                str_base = k_vec_base[j];
                base *= 1000.0;
            }

            if(vec_er_src_size_[i] > 0) {
                compression_rate = vec_er_des_size_[i]/vec_er_src_size_[i] * 100;

                str_compression_rate = "(" + std::to_string(compression_rate) + "%)";
            }

            str_description = std::to_string(encoded_file_size) + " " + str_base + str_compression_rate;

            break;

        case ENCODE_RESULT_STATUS::FAILED_NOT_VALID_FILE :

            str_description = "Not Valid File";

            break;
        default :

            str_description = "Undefiend Error";

            break;
        }

        std::cout << std::setw(11) << str_file_name;
        std::cout << std::setw(2) << "|";
        std::cout << std::setw(8) << str_result_status;
        std::cout << std::setw(2) << "|";
        std::cout << std::setw(10) << str_description;
        std::cout << std::endl;
    }

    std::cout << std::endl << "USED THREADS COUNT : " << used_threads_count_ << std::endl;
}

Encoder* LameEncoderLibrary::ReturnEncoderBySourceType(const char* file_type) {

    if(strcmp(file_type, ".wav") == 0) {
        return new LameWavEncoder();
    }

    std::cerr << "Failed to find a proper encoder library" << std::endl;

    return new NullEncoder();
}

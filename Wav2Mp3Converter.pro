TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

osx {
    LIBS += -L/usr/local/Cellar/lame/3.100/lib -lmp3lame \
            -lpthread \

    INCLUDEPATH +=  "/Users/Abel/Shared/2019_2/Europe/coding_test/cinemo/lib/lame-3.100/include" \
}

win32 {
    LIBS += -L"C:\Users\tredi\Desktop\lame-3.100.tar\lame-3.100\output\Release" -llibmp3lame-static -llibmpghip-static \
            -L"C:\Users\tredi\Desktop\pthread" -lpthreadVC2

    INCLUDEPATH +=  "C:\Users\tredi\Desktop\lame-3.100.tar\lame-3.100\include" \
                    "C:\Users\tredi\Desktop\pthread" \
                    "C:\Users\tredi\Desktop\dirent\include"
}

SOURCES += \
        Wav2Mp3Converter.cpp \

HEADERS += \
    encoder_library.hpp \
    file_explorer.hpp \
    my_wav.hpp

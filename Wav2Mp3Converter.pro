TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# For test and debug only
osx {
    LIBS += -L/usr/local/Cellar/lame/3.100/lib -lmp3lame \
            -lpthread \

    INCLUDEPATH +=  "/Users/Abel/Shared/2019_2/Europe/coding_test/cinemo/lib/lame-3.100/include" \
}

# For windows build configuration
win32 {
    # HERE YOU CAN SET PATH
    LIBS += -L"$$PWD\external_libs\lame\lib" -llibmp3lame-static -llibmpghip-static \
            -L"$$PWD\external_libs\pthread\lib" -lpthreadVC2 \
            -L"$$PWD\external_libs\msvc\lib" -lmsvcrt -lmsvcmrt

    # HERE YOU CAN SET PATH
    INCLUDEPATH +=  "$$PWD\external_libs\lame\include" \
                    "$$PWD\external_libs\pthread\include" \
                    "$$PWD\external_libs\dirent\include"

    CONFIG += static
    QMAKE_LFLAGS += "/nodefaultlib:MSVCRT"
}

SOURCES += \
    Wav2Mp3Converter.cpp \
    encoder_library.cpp \
    file_explorer.cpp \
    my_wav.cpp

HEADERS += \
    encoder_library.hpp \
    file_explorer.hpp \
    my_wav.hpp

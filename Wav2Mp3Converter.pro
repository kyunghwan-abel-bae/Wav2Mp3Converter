TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS += -std=c++11

LIBS += -L/usr/local/Cellar/lame/3.100/lib -lmp3lame \
        -lpthread \


INCLUDEPATH +=  "/Users/Abel/Shared/2019_4/Sweden/coding_test/cinemo/lib/lame-3.100/include" \

SOURCES += \
        Wav2Mp3Converter.cpp

HEADERS += \
    encoder_library.hpp \
    file_explorer.hpp \
    my_wav.hpp

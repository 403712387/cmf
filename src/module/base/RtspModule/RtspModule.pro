QT       += core
QT       -= gui
CONFIG += c++11
TARGET = RtspManager
TEMPLATE = lib
DESTDIR = ../../../bin/cmf/lib/module
OBJECTS_DIR = ../../../output/RtspManager
MOC_DIR = ../../../output/RtspManager
#QMAKE_LFLAGS += -fPIC

INCLUDEPATH += ../Common/src \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/Qt \
            ../Common/src/message \
            ../Common/src/info \
            ../Common/src/common \
            ../Common/src/SystemInfo \
            ./src \
            ./src/helper \
            ../CoreModule/src \
            ./src/protocol/librtsp/test \
            ./src/protocol/librtsp/include \
            ./src/protocol/librtsp/source \
            ./src/protocol/librtsp/source/client \
            ./src/protocol/librtsp/source/server \
            ./src/protocol/librtsp/source/sdp

win32 {
LIBS += -L../../../bin/cmf/lib/module -lCommon -lCoreModule -lCommon \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/jsoncpp -ljsoncpp
}

unix {
LIBS += -L../../../bin/cmf/lib/module -lCommon -lCoreModule -lCommon \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/linux64/jsoncpp  -ljsoncpp
}

HEADERS += \
    src/*.h \
    src/helper/*.h \
    ./src/protocol/librtsp/test/*.h \
    ./src/protocol/librtsp/include/*.h \
    ./src/protocol/librtsp/source/*.h \
    ./src/protocol/librtsp/source/client/*.h \
    ./src/protocol/librtsp/source/server/*.h \
    ./src/protocol/librtsp/source/server/aio/*.h \
    ./src/protocol/librtsp/source/sdp/*.h

SOURCES += \
    src/*.cpp \
    src/helper/*.cpp \
    ./src/protocol/librtsp/test/*.c \
    ./src/protocol/librtsp/include/*.c \
    ./src/protocol/librtsp/source/*.c \
    ./src/protocol/librtsp/source/client/*.c \
    ./src/protocol/librtsp/source/server/*.c \
    ./src/protocol/librtsp/source/server/aio/*.c \
    ./src/protocol/librtsp/source/sdp/*.c

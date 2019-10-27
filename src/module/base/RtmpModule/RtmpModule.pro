QT       += core
QT       -= gui
CONFIG += c++11
TARGET = RtmpManager
TEMPLATE = lib
DESTDIR = ../../../bin/cmf/lib/module
OBJECTS_DIR = ../../../output/RtmpManager
MOC_DIR = ../../../output/RtmpManager
DEFINES += _FLASH_HANDSHAKE_
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
            ./src/protocol/librtmp/aio \
            ./src/protocol/librtmp/include \
            ./src/protocol/librtmp/source \
            ./src/protocol/librtmp/test

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
    ./src/protocol/librtmp/aio/*.h \
    ./src/protocol/librtmp/include/*.h \
    ./src/protocol/librtmp/source/*.h \
    ./src/protocol/librtmp/test/*.h

SOURCES += \
    src/*.cpp \
    src/helper/*.cpp \
    ./src/protocol/librtmp/aio/*.c \
    ./src/protocol/librtmp/include/*.c \
    ./src/protocol/librtmp/source/*.c \
    ./src/protocol/librtmp/test/*.c

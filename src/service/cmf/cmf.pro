QT       += core
QT       -= gui
CONFIG += c++11 console
TARGET = cmf
TEMPLATE = app
DESTDIR = ../../bin/cmf
OBJECTS_DIR = ../../output/cmf
MOC_DIR = ../../output/cmf
DEFINES += QT_NO_VERSION_TAGGING
#QMAKE_LFLAGS += -fPIC

INCLUDEPATH += \
            ../../module/base/Common/src \
            ../../module/base/Common/src/message \
            ../../module/base/Common/src/perftool \
            ../../module/base/Common/src/info \
            ../../module/base/Common/src/common \
            ../../module/base/CoreModule/src \
            ../../module/base/HttpModule/src \
            ../../module/base/ServiceStatusModule/src \
            ../../module/base/ConfigureModule/src \
            ../../../thirdparty/include

win32 {
LIBS += -L../../bin/cmf/lib/module -lCommon -lCoreModule -lHttpModule -lServiceStatusModule -lConfigureModule \
        -L../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../thirdparty/lib/windows64/jsoncpp  -ljsoncpp
}

unix {
LIBS += -L../../bin/cmf/lib/module -lCommon -lCoreModule -lHttpModule -lServiceStatusModule -lConfigureModule \
        -L../../../thirdparty/lib/linux64/jsoncpp  -ljsoncpp \
        -L../../../thirdparty/lib/linux64/log4cplus -llog4cplus \
        -L../../../thirdparty/lib/linux64/curl -lcurl \
        -L../../../../thirdparty/lib/linux64/gperftools/static -lprofiler -ltcmalloc \
        -L../../../thirdparty/lib/linux64/libmicrohttpd  -lmicrohttpd \
        -L../../../thirdparty/lib/linux64/Qt -lQt5Core -licudata -licui18n -licuuc \
        -luuid
}

SOURCES += \
    src/*.cpp

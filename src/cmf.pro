TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += \
        ./module/base/Common/Common.pro \
        ./module/base/CoreModule/CoreModule.pro \
        ./module/base/HttpModule/HttpModule.pro \
        ./module/base/RtmpModule/RtmpModule.pro \
        ./module/base/RtspModule/RtspModule.pro \
        ./module/base/ServiceStatusModule/ServiceStatusModule.pro \
        ./module/base/ConfigureModule/ConfigureModule.pro \
        ./service/cmf/cmf.pro

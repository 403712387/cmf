#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <sstream>
#include "Log.h"
#include "Common.h"
#include "HttpHelper.h"
#include "JsonHelper.h"
#include "HttpModule.h"
#include "CPUStatistic.h"
#include "DiskStatistic.h"
#include "PlatformInfo.h"
#include "SystemInfo.h"
#include "ConfigureInfo.h"
#include "ProcessStatistic.h"
#include "MemoryStatistic.h"
#include "ServiceStatusInfo.h"
#include "ServiceControlMessage.h"
#include "GetServiceStatusMessage.h"
#include "ConfigureInfoMessage.h"
#include "jsoncpp/json.h"
#include "perftool/perftool.h"

struct TPostStatus
{
    bool status = false;
    std::string buffer;
};

HttpModule *HttpModule::mHttpService = NULL;
HttpModule::HttpModule(MessageRoute *messageRoute)
    :BaseProcess(messageRoute, "HttpModule")
{
    mHttpService = this;

    // 订阅消息
    subscribeMessage(Config_Message);
}

bool HttpModule::init()
{
    LOG_I(mClassName, "begin init");
    LOG_I(mClassName, "end init");
    return true;
}

void HttpModule::beginWork()
{
    LOG_I(mClassName, "begin work");
}

// 反初始化
void HttpModule::uninit()
{
    stopListen();
    BaseProcess::uninit();
    LOG_I(mClassName, "uninit http manager, port:" << mHttpPort);
}

// 处理消息的函数
std::shared_ptr<BaseResponse> HttpModule::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    switch(message->getMessageType())
    {
    case Config_Message:        // 配置信息
        response = onProcessConfigMessage(message);
    }

    return response;
}

// 处理消息的回应
void HttpModule::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{

}

// 初始化服务
bool HttpModule::startListen(int port)
{
    if (isListening())
    {
        LOG_W(mClassName, "http server already run, listen port:" << mHttpPort << ", want listen port:" << port);
        return false;
    }

    // 监听端口
    unsigned int flags = MHD_USE_EPOLL_INTERNAL_THREAD | MHD_USE_ERROR_LOG;

    // windows下只支持select
#ifdef WIN32
    flags = MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG;
#endif

    mHttpHandle = MHD_start_daemon(flags, port, NULL, NULL, HttpModule::onProcessCallback, NULL, MHD_OPTION_END);
    if (NULL == mHttpHandle)
    {
        LOG_F(mClassName, "start http service fail, port:" << port);
        return false;
    }

    mHttpPort = port;
    LOG_I(mClassName, "start http service, port:" << port);
    return true;
}

// 停止监听
bool HttpModule::stopListen()
{
    if (!isListening())
    {
        LOG_W(mClassName, "http not listen");
        return false;
    }

    MHD_stop_daemon(mHttpHandle);
    mHttpHandle = NULL;
    LOG_I(mClassName, "stop http listen");
    return true;
}

// 是否正在监听
bool HttpModule::isListening()
{
    return (NULL != mHttpHandle);
}

// 处理配置消息
std::shared_ptr<BaseResponse> HttpModule::onProcessConfigMessage(std::shared_ptr<BaseMessage> message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<ConfigureInfoMessage> configureMessage = std::dynamic_pointer_cast<ConfigureInfoMessage>(message);
    mConfigureInfo = configureMessage->getConfigureInfo();

    // 如果http端口发生改变，则要重新监听
    if (mHttpPort != mConfigureInfo->getHttpPort())
    {
        if (isListening())
        {
            stopListen();
        }
        mHttpPort = mConfigureInfo->getHttpPort();
        startListen(mHttpPort);
    }

    return response;
}

// 处理http请求回调
int HttpModule::onProcessCallback(void *cls, MHD_Connection *connection, const char *url,const char *method, const char *version,
              const char *upload_data, size_t *upload_data_size, void **con_cls)
{
    return mHttpService->onProcess(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
}

// 处理http请求
int HttpModule::onProcess(void *cls, MHD_Connection *connection, const char *url,const char *method, const char *version,
              const char *upload_data, size_t *upload_data_size, void **con_cls)
{
    QString requestUrl(url);
    std::string body;
    HttpBodyType httpBodyType = Http_Body_Json;
    bool ret = getHttpBody(cls, connection, url, method, version, upload_data, upload_data_size, con_cls, body);
    if (false == ret)
    {
        return MHD_YES;
    }

    std::string response;
    if (requestUrl.compare("/index", Qt::CaseInsensitive) == 0) // index页面
    {
        response = onProcessIndex(body);
        httpBodyType = Http_Body_Html;
    }
    else if (requestUrl.compare("/get/configure", Qt::CaseInsensitive) == 0) // 获取配置信息
    {
        response = onGetConfigureInfo(body);
    }
    else if (requestUrl.compare("/set/configure", Qt::CaseInsensitive) == 0) // 获取配置信息
    {
        response = onSetConfigureInfo(body);
    }
    else if (requestUrl.compare("/get/service/status", Qt::CaseInsensitive) == 0) // 获取服务状态
    {
        response = onGetServiceStatus(body);
    }
    else if (requestUrl.compare("/control/service", Qt::CaseInsensitive) == 0)  // 服务控制
    {
        response = onControlService(body);
    }
    else if (requestUrl.compare("/cpu/profile/start", Qt::CaseInsensitive) == 0)  // 开始性能分析
    {
        response = onStartCpuProfile(body);
    }
    else if (requestUrl.compare("/cpu/profile/stop", Qt::CaseInsensitive) == 0)  //  停止性能分析
    {
        response = onStopCpuProfile(body);
    }
    else if (requestUrl.compare("/heap/profile/start", Qt::CaseInsensitive) == 0)  // 开始内存分析
    {
        response = onStartCpuProfile(body);
    }
    else if (requestUrl.compare("/heap/profile/stop", Qt::CaseInsensitive) == 0)  //  停止内存分析
    {
        response = onStopCpuProfile(body);
    }
    else if (requestUrl.compare("/get/cpu/statstic", Qt::CaseInsensitive) == 0)  //  获取cpu信息
    {
        response = onGetCPUStatisticInfo();
    }
    else if (requestUrl.compare("/get/disk/statstic", Qt::CaseInsensitive) == 0)  //  获取磁盘信息
    {
        response = onGetDiskStatisticInfo();
    }
    else if (requestUrl.compare("/get/process/statistic", Qt::CaseInsensitive) == 0)  //  获取进程信息
    {
        response = onGetProcessStatisticInfo();
    }
    else if (requestUrl.compare("/get/memory/statistic", Qt::CaseInsensitive) == 0)  //  获取内存信息
    {
        response = onGetMemoryStatisticInfo();
    }
    else if (requestUrl.compare("/get/platform/info", Qt::CaseInsensitive) == 0)  //  获取平台信息
    {
        response = onGetPlatformInfo();
    }
    else if (requestUrl.startsWith("/download/", Qt::CaseInsensitive))  //  下载文件
    {
        response = onDownloadFile(requestUrl.toStdString());
        httpBodyType = Http_Body_File;
    }
    else
    {
        LOG_E(mClassName, "not find http request process, url " << url);
        response = getResponseBody(404, "don not touch me");
    }

    ret = sendResponse(connection, response, url, httpBodyType);
    return ret ? MHD_YES : MHD_NO;
}

// 获取配置信息
std::string HttpModule::onGetConfigureInfo(std::string &body)
{
    std::string result;
    if (NULL == mConfigureInfo.get())
    {
        result = getResponseBody(500, "get configure fail");
    }
    else
    {
        result = mConfigureInfo->toJson().toStyledString();
    }
    return result;
}

// 设置配置信息
std::string HttpModule::onSetConfigureInfo(std::string &body)
{
    bool ret = JsonHelper::parseConfigure(body, mConfigureInfo);
    if (!ret)
    {
        return getResponseBody(400, "parse json fail");
    }

    // 发送更新配置消息
    std::shared_ptr<ConfigureInfoMessage> message = std::make_shared<ConfigureInfoMessage>(mConfigureInfo);
    sendMessage(message);

    return getResponseBody(200, "OK");
}

// 获取服务状态
std::string HttpModule::onGetServiceStatus(std::string &body)
{
    std::string result;

    // 获取服务器状态
    std::shared_ptr<GetServiceStatusMessage> message = std::make_shared<GetServiceStatusMessage>(Sync_Trans_Message);
    std::shared_ptr<BaseResponse> response = sendMessage(message);
    std::shared_ptr<GetServiceStatusResponse> serviceResponse = std::dynamic_pointer_cast<GetServiceStatusResponse>(response);
    if (NULL == serviceResponse.get())
    {
        LOG_E(mClassName, "get service status fail, response is NULL");
        result = getResponseBody(500, "get service status fail");
        return result;
    }

    // 获取服务状态信息
    Json::Value serviceStatus = serviceResponse->getServiceStatusInfo()->toJson();
    result = serviceStatus.toStyledString();

    return result;
}

// 服务控制
std::string HttpModule::onControlService(std::string &body)
{
    std::string result;
    ServiceOperateType type = JsonHelper::parseServiceControl(body);
    std::shared_ptr<ServiceControlMessage> message = std::make_shared<ServiceControlMessage>(type);
    sendMessage(message);
    result = getResponseBody(200, "OK");
    return result;
}

// 发送回应
bool HttpModule::sendResponse(MHD_Connection *connection, std::string info, std::string url, HttpBodyType bodyType)
{
    // 返回结果
    std::string responseJson = info;
    struct MHD_Response *response = MHD_create_response_from_buffer(responseJson.size(),(void*)responseJson.c_str(), MHD_RESPMEM_MUST_COPY);

    std::string httpBodyType = Common::getHttpTypeName(bodyType);
    if (MHD_add_response_header(response, "Content-Type", httpBodyType.c_str()) == MHD_NO)
    {
        LOG_E(mClassName, "create response head fail when process request, url:" << url);
        return false;
    }
    if (MHD_queue_response(connection, MHD_HTTP_OK, response) == MHD_NO)
    {
        LOG_E(mClassName, "put response fail when process request, url:" << url);
        return false;
    }
    if (Http_Body_File == bodyType)
    {
        std::string fileName = QFileInfo(url.c_str()).baseName().toStdString();
        std::string value = "attachment;filename=" + fileName;
        MHD_add_response_header(response, "Content-Disposition", value.c_str());
    }

    if (Http_Body_Image != bodyType)
    {
        LOG_I(mClassName, "send response:" << info);
    }

    MHD_destroy_response(response);
    return true;
}

// 获取http的body
bool HttpModule::getHttpBody(void *cls, MHD_Connection *connection, const char *url,const char *method, const char *version,
                 const char *upload_data, size_t *upload_data_size, void **ptr, std::string &body)
{
    TPostStatus *post = NULL;
    post = (struct TPostStatus*)*ptr;

    if (NULL == post)
    {
        post = new TPostStatus();
        post->status = false;
        *ptr = post;
    }

    if (!post->status)
    {
        post->status = true;
        return false;
    }
    else
    {
        if (*upload_data_size != 0)
        {
            std::string data = std::string(upload_data, *upload_data_size);
            post->buffer += data;
            *upload_data_size = 0;
            return false;
        }
        else
        {
            body = post->buffer;
        }
    }

    if (post != NULL)
    {
        delete post;
    }
    return true;
}
// 开始cpu性能分析
std::string HttpModule::onStartCpuProfile(std::string &body)
{
    std::string path = PerfTool::startCPUProfiler();
    path = QString::fromStdString(path).replace("./html/", "/download/").toStdString();
    LOG_I(mClassName, "begin cpu profile");
    return getResponseBody(200, path);
}

// 停止cpu性能分析
std::string HttpModule::onStopCpuProfile(std::string &body)
{
    std::string path = PerfTool::startCPUProfiler();
    if (path.empty())
    {
        return getResponseBody(400, "cpu profile not start");
    }

    LOG_I(mClassName, "end cpu profile");
    path = QString::fromStdString(path).replace("./html/", "/download/").toStdString();
    return getResponseBody(200, path);
}

// 开始内存性能分析
std::string HttpModule::onStartHeapProfile(std::string &body)
{
    std::string path = PerfTool::startHeapProfiler();
    path = QString::fromStdString(path).replace("./html/", "/download/").toStdString();
    LOG_I(mClassName, "begin heap profile");
    return getResponseBody(200, path);
}

// 停止内存性能分析
std::string HttpModule::onStopHeapProfile(std::string &body)
{
    std::string path = PerfTool::startHeapProfiler();
    if (path.empty())
    {
        return getResponseBody(400, "heap profile not start");
    }

    LOG_I(mClassName, "end heap profile");
    path = QString::fromStdString(path).replace("./html/", "/download/").toStdString();
    return getResponseBody(200, path);
}

// 下载文件
std::string HttpModule::onDownloadFile(std::string url)
{
    url = QString::fromStdString(url).replace("/download/", "./html/").toStdString();
    std::string data = Common::readFile(url);
    if (data.empty())
    {
        data = getResponseBody(404, "not file " + url);
    }
    return data;
}

// 获取CPU使用情况
std::string HttpModule::onGetCPUStatisticInfo()
{
    std::shared_ptr<CPUStatistic> result = SystemInfo::getCpuInfo();
    if (NULL == result.get())
    {
        return getResponseBody(500, "get info fail");
    }
    return result->toJson().toStyledString();
}

// 获取进程统计信息
std::string HttpModule::onGetProcessStatisticInfo()
{
    std::shared_ptr<ProcessStatistic> result = SystemInfo::getProcessInfo();
    if (NULL == result.get())
    {
        return getResponseBody(500, "get info fail");
    }
    return result->toJson().toStyledString();
}

// 获取磁盘使用情况
std::string HttpModule::onGetDiskStatisticInfo()
{
    std::vector<std::shared_ptr<DiskStatistic>> disks = SystemInfo::getDiskInfo();
    if (disks.empty())
    {
         return getResponseBody(500, "get info fail");
    }
    Json::Value result;
    for (int i = 0; i < disks.size(); ++i)
    {
        std::shared_ptr<DiskStatistic> disk = disks.at(i);
        result["disk"][i] = disk->toJson();
    }
    return result.toStyledString();
}

// 获取内存使用情况
std::string HttpModule::onGetMemoryStatisticInfo()
{
    std::shared_ptr<MemoryStatistic> result = SystemInfo::getMemoryInfo();
    if (NULL == result.get())
    {
        return getResponseBody(500, "get info fail");
    }
    return result->toJson().toStyledString();
}

// 获取平台信息
std::string HttpModule::onGetPlatformInfo()
{
    std::shared_ptr<PlatformInfo> result = SystemInfo::getPlatformInfo();
    if (NULL == result.get())
    {
        return getResponseBody(500, "get info fail");
    }
    return result->toJson().toStyledString();
}

// 获取呼应的body
std::string HttpModule::getResponseBody(int status, std::string reason)
{
    std::string responseJson = "{\"status\":" + std::to_string(status) + ", \"reason\":\"" + reason + "\"}";
    return responseJson;
}

// index页面
std::string HttpModule::onProcessIndex(std::string &body)
{
    std::string result = "Welcome to cmf(c++ media framework)";
    return result;
}

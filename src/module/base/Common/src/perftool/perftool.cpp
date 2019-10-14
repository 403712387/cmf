#include <QtCore/QtGlobal>
#include <QtCore/QFileInfo>
#include <QtCore/QByteArray>
#include "Log.h"
#include "perftool.h"
#include "Common.h"
#include "gperftools/profiler.h"
#include "gperftools/heap-profiler.h"

std::string PerfTool::mClassName = "PerfTool";
bool PerfTool::mCpuProfiler = false;
bool PerfTool::mHeapProfiler = false;
std::string PerfTool::mCpuProfilerFile = "./html/perf/cpu.prof";
std::string PerfTool::mHeapProfilerFile = "./html/perf/heap.prof";

// 开始性能检测
std::string PerfTool::startCPUProfiler()
{
#ifdef WIN32
    return "";
#else
    // 判断是不是已经开启
    if (true == mCpuProfiler)
    {
        return mCpuProfilerFile;
    }

    // 创建目录
    std::string path = QFileInfo(mCpuProfilerFile.c_str()).path().toStdString();
    Common::createPath(path);

    mCpuProfiler = true;
    LOG_I(mClassName, "start cpu prifiler, file:" << mCpuProfilerFile);
    ProfilerStart(mCpuProfilerFile.c_str());
    return mCpuProfilerFile;
#endif
}

// 停止性能检测
std::string PerfTool::stopCPUProfiler()
{
#ifdef WIN32
    return "";
#else
    if (false == mCpuProfiler)
    {
        return "";
    }
    mCpuProfiler = false;
    LOG_I(mClassName, "stop cpu prifiler");
    ProfilerStop();
    return mCpuProfilerFile;
#endif
}

// 开始分析堆栈
std::string PerfTool::startHeapProfiler()
{
#ifdef WIN32
    return "";
#else
    // 判断是不是已经开启
    if (true == mHeapProfiler)
    {
        return mHeapProfilerFile;
    }

    // 创建目录
    std::string path = QFileInfo(mHeapProfilerFile.c_str()).path().toStdString();
    Common::createPath(path);

    mHeapProfiler = true;
    LOG_I(mClassName, "start heap prifiler, file:" << mHeapProfilerFile);
    HeapProfilerStart(mHeapProfilerFile.c_str());
    return mHeapProfilerFile;
#endif
}

// 停止堆栈分析
std::string PerfTool::stopHeapProfiler()
{
#ifdef WIN32
    return "";
#else
    if (false == mHeapProfiler)
    {
        return "";
    }
    mHeapProfiler = false;
    LOG_I(mClassName, "stop heap prifiler");
    HeapProfilerStop();
    return mHeapProfilerFile;
#endif
}

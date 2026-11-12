#include <cstdio>
#include <ctime>
#include <utility>
#include <vector>
#include <cassert>
#include <cstring>
#include <string>
#include <cstdlib>
#define private public
#include "../../game/src/TrafficProfiler.h"
#undef private
static std::string ReadAll(const char* path)
{
    FILE* f = fopen(path, "r"); assert(f);
    std::string s; char data[512]; size_t n;
    while ((n = fread(data, 1, sizeof(data), f)) != 0) s.append(data, n);
    fclose(f); return s;
}
int main()
{
    std::string logPath = std::string(getenv("TEMP") ? getenv("TEMP") : ".") + "/mcncc_traffic_regression.log";
    const char* path = logPath.c_str();
    {
        TrafficProfiler p;
        assert(TrafficProfiler::instance_ptr() == &p);
        assert(p.m_pfProfileLogFile == 0);
        assert(p.m_aTrafficVec[0].size() == 256 && p.m_aTrafficVec[1].size() == 256);
        assert(!p.Flush());
        assert(p.Initialize(3600, path));
        assert(p.Report(TrafficProfiler::IODIR_INPUT, 255, 7));
        assert(p.Report(TrafficProfiler::IODIR_INPUT, 255, 9));
        assert(p.Report(TrafficProfiler::IODIR_OUTPUT, 255, 3));
        assert(p.m_dwTotalTraffic == 19 && p.m_dwTotalPacket == 2);
        assert(p.Flush());
        std::string report = ReadAll(path);
        assert(report.find("Total traffic: 19 bytes\nTotal used packet: 2\n") != std::string::npos);
        assert(report.find("255\t2\t16\t\t8\n") != std::string::npos);
        assert(report.find("255\t1\t3\t\t3\n") != std::string::npos);
        std::string separator = "------------------ Output -----------------\n";
        size_t first = report.find(separator);
        assert(first != std::string::npos && report.find(separator, first+separator.size()) != std::string::npos);
        assert(p.m_dwTotalTraffic == 0 && p.m_dwTotalPacket == 0);
        assert(p.m_aTrafficVec[0][255].second == 0);
        p.m_dwFlushCycle = 0;
        assert(p.Report(TrafficProfiler::IODIR_OUTPUT, 1, 4));
        assert(p.m_dwTotalTraffic == 0 && p.m_dwTotalPacket == 0);
        p.m_dwFlushCycle = 3600;
        assert(p.Report(TrafficProfiler::IODIR_OUTPUT, 2, 0x80000000UL));
        assert(p.Flush());
        assert(ReadAll(path).find("Total traffic: -2147483648 bytes") != std::string::npos);
    }
    assert(TrafficProfiler::instance_ptr() == 0);
    remove(path);
    puts("traffic profiler regression passed");
}

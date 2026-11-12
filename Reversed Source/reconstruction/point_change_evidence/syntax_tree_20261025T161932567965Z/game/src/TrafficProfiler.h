#ifndef MCNCC_TRAFFIC_PROFILER_H
#define MCNCC_TRAFFIC_PROFILER_H
#include <cstdio>
#include <ctime>
#include <utility>
#include <vector>
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
class TrafficProfiler : public singleton<TrafficProfiler>
{
public:
    enum IODirection { IODIR_INPUT = 0, IODIR_OUTPUT, IODIR_MAX };
    TrafficProfiler();
    virtual ~TrafficProfiler();
    bool Initialize(DWORD dwFlushCycle, const char* pszLogFileName);
    bool Report(IODirection dir, BYTE byHeader, DWORD dwSize)
    {
        ComputeTraffic(dir, byHeader, dwSize);
        if ((DWORD)(time(NULL) - m_tmProfileStartTime) >= m_dwFlushCycle)
            return Flush();
        return true;
    }
    bool Flush();
private:
    void InitializeProfiling();
    void ComputeTraffic(IODirection dir, BYTE byHeader, DWORD dwSize)
    {
        TrafficInfo& info = m_aTrafficVec[dir][byHeader];
        m_dwTotalTraffic += dwSize;
        m_dwTotalPacket += !info.second;
        info.first += dwSize;
        ++info.second;
    }
    typedef std::pair<DWORD, DWORD> TrafficInfo;
    typedef std::vector<TrafficInfo> TrafficVec;
    FILE* m_pfProfileLogFile;
    DWORD m_dwFlushCycle;
    time_t m_tmProfileStartTime;
    DWORD m_dwTotalTraffic;
    DWORD m_dwTotalPacket;
    TrafficVec m_aTrafficVec[IODIR_MAX];
};
#endif

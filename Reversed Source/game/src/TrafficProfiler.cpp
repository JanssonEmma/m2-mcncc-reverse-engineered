// Recovered target profiler; see reconstruction/traffic-profiler-ghidra.json.
#include "TrafficProfiler.h"
TrafficProfiler::TrafficProfiler()
{
    m_aTrafficVec[0].resize(256);
    m_aTrafficVec[1].resize(256);
    m_pfProfileLogFile = NULL;
}
TrafficProfiler::~TrafficProfiler()
{
    if (m_pfProfileLogFile)
        fclose(m_pfProfileLogFile);
}
bool TrafficProfiler::Initialize(DWORD dwFlushCycle, const char* pszLogFileName)
{
    m_pfProfileLogFile = fopen(pszLogFileName, "w");
    if (!m_pfProfileLogFile)
        return false;
    m_dwFlushCycle = dwFlushCycle;
    InitializeProfiling();
    return true;
}
bool TrafficProfiler::Flush()
{
    if (!m_pfProfileLogFile)
        return false;
    fprintf(m_pfProfileLogFile, "# Profile Start: %s", ctime(&m_tmProfileStartTime));
    fprintf(m_pfProfileLogFile, "Total traffic: %d bytes\n", static_cast<int>(m_dwTotalTraffic));
    fprintf(m_pfProfileLogFile, "Total used packet: %d\n", static_cast<int>(m_dwTotalPacket));
    fprintf(m_pfProfileLogFile, "------------------ Input ------------------\n");
    for (int dir = 0; dir < IODIR_MAX; ++dir)
    {
        fprintf(m_pfProfileLogFile, "Packet\tCount\tTotal Size\tAverage\n");
        BYTE header = 0;
        for (TrafficVec::iterator it = m_aTrafficVec[dir].begin();
             it != m_aTrafficVec[dir].end(); ++it, ++header)
            if (it->second)
                fprintf(m_pfProfileLogFile, "%d\t%d\t%d\t\t%d\n", static_cast<int>(header),
                        static_cast<int>(it->second), static_cast<int>(it->first),
                        static_cast<int>(it->first / it->second));
        fprintf(m_pfProfileLogFile, "------------------ Output -----------------\n");
    }
    time_t current = time(NULL);
    fprintf(m_pfProfileLogFile, "# Profile End(Flush): %s", ctime(&current));
    fflush(m_pfProfileLogFile);
    InitializeProfiling();
    return true;
}
void TrafficProfiler::InitializeProfiling()
{
    m_tmProfileStartTime = time(NULL);
    m_dwTotalPacket = 0;
    m_dwTotalTraffic = 0;
    TrafficInfo empty(0, 0);
    for (int dir = 0; dir < IODIR_MAX; ++dir)
        for (TrafficVec::iterator it = m_aTrafficVec[dir].begin();
             it != m_aTrafficVec[dir].end(); ++it)
            *it = empty;
}

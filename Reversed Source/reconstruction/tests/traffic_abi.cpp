#include <cstdio>
#include <ctime>
#include <utility>
#include <vector>
#include <cstddef>
#define private public
#include "../../game/src/TrafficProfiler.h"
#undef private
#define ABI(name, expression) typedef char name[(expression) ? 1 : -1]
ABI(profiler_size, sizeof(TrafficProfiler) == 48);
ABI(profiler_file, offsetof(TrafficProfiler, m_pfProfileLogFile) == 4);
ABI(profiler_cycle, offsetof(TrafficProfiler, m_dwFlushCycle) == 8);
ABI(profiler_time, offsetof(TrafficProfiler, m_tmProfileStartTime) == 12);
ABI(profiler_bytes, offsetof(TrafficProfiler, m_dwTotalTraffic) == 16);
ABI(profiler_packets, offsetof(TrafficProfiler, m_dwTotalPacket) == 20);
ABI(profiler_vectors, offsetof(TrafficProfiler, m_aTrafficVec) == 24);
ABI(profiler_vector_size, sizeof(TrafficProfiler::TrafficVec) == 12);
ABI(profiler_entry_size, sizeof(TrafficProfiler::TrafficInfo) == 8);

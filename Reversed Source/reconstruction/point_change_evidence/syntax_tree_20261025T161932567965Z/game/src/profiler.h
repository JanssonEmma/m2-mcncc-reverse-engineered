// Partial old profiler behavior with complete data/virtual declarations.
#ifndef MCNCC_GAME_PROFILER_H
#define MCNCC_GAME_PROFILER_H
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cstddef>
#include <cstdio>
#include <ext/hash_map>
#include "../../common/singleton.h"
#include "../../common/stl.h"
#include "../../libthecore/include/utils.h"
class CProfiler : public singleton<CProfiler> {
public:
    enum { STACK_DATA_MAX_NUM = 64 };
    typedef struct SProfileStackData {
        int iCallStep;
        long iStartTime;
        long iEndTime;
        std::string strName;
    } TProfileStackData;
    typedef struct SProfileAccumData {
        int iStartTime;
        int iCallingCount;
        int iCollapsedTime;
        int iDepth;
        std::string strName;
    } TProfileAccumData;
    struct stringhash {
        std::size_t operator()(const std::string& value) const;
    };
    typedef __gnu_cxx::hash_map<std::string, TProfileAccumData, stringhash,
        std::equal_to<std::string>, std::allocator<TProfileAccumData> > TProfileAccumDataMap;
    CProfiler();
    virtual ~CProfiler();
    void Initialize();
    void Clear();
    void Push(const char* name);
    void Pop(const char* name);
    void PushAccum(const char* name);
    void PopAccum(const char* name);
    void Log(const char* filename);
    void Print(FILE* stream = stderr);
    void PrintOneStackData(const char* name);
    void PrintOneAccumData(const char* name);
protected:
    bool GetProfileStackDataPointer(const char* name, TProfileStackData** result);
    int m_ProfileStackDataCount;
    TProfileStackData m_ProfileStackDatas[STACK_DATA_MAX_NUM];
    TProfileAccumDataMap m_ProfileAccumDataMap;
    std::vector<TProfileAccumData*> m_vec_Accum;
    int m_iAccumDepth;
    int m_iCallStep;
};

inline std::size_t CProfiler::stringhash::operator()(const std::string& value) const {
    return ::stringhash()(value);
}
inline CProfiler::CProfiler() : m_ProfileAccumDataMap(100) { Initialize(); }
inline CProfiler::~CProfiler() {}
inline void CProfiler::Initialize() {
    m_iAccumDepth=0; m_ProfileStackDataCount=0; m_iCallStep=0;
}
inline void CProfiler::PushAccum(const char* name) {
    TProfileAccumDataMap::iterator it=m_ProfileAccumDataMap.find(name);
    if(it==m_ProfileAccumDataMap.end()) {
        TProfileAccumData record;
        // The old temporary's start time is indeterminate until the final clock read.
        record.iCollapsedTime=0; record.iCallingCount=0;
        record.strName=name; record.iDepth=m_iAccumDepth;
        m_ProfileAccumDataMap.insert(std::make_pair(std::string(name),record));
        it=m_ProfileAccumDataMap.find(name);
        m_vec_Accum.push_back(&it->second);
    }
    it->second.iStartTime=static_cast<int>(get_dword_time());
    m_iAccumDepth=static_cast<int>(static_cast<unsigned int>(m_iAccumDepth)+1U);
}
inline void CProfiler::Print(FILE* stream) {
    for(int i=0;i<m_ProfileStackDataCount;++i) {
        const TProfileStackData& record=m_ProfileStackDatas[i];
        for(int step=0;step<record.iCallStep;++step) std::fprintf(stream,"\t");
        int elapsed=static_cast<int>(static_cast<unsigned long>(record.iEndTime)-static_cast<unsigned long>(record.iStartTime));
        std::fprintf(stream,"%-24s: %d\n",record.strName.c_str(),elapsed);
    }
    std::fprintf(stream,"Name                                 Call/Load       Ratio\n");
    std::string line;
    for(std::size_t i=0;i<m_vec_Accum.size();++i) {
        TProfileAccumData* record=m_vec_Accum[i];
        line.assign(static_cast<unsigned int>(record->iDepth)*5U,' ');
        line+=record->strName;
        double ratio=record->iCallingCount?static_cast<double>(record->iCollapsedTime)/record->iCallingCount:0.0;
        std::fprintf(stream,"%-30s %10d/%-10d %.2f\n",line.c_str(),record->iCallingCount,record->iCollapsedTime,ratio);
    }
}


// Caller-inline evidence:0814fac8..0814fb54 and081512fd..08151321.
inline void CProfiler::PopAccum(const char* name) {
    TProfileAccumDataMap::iterator it=m_ProfileAccumDataMap.find(name);
    if(it==m_ProfileAccumDataMap.end()) return;
    TProfileAccumData& record=it->second;
    unsigned int elapsed=static_cast<unsigned int>(get_dword_time())-static_cast<unsigned int>(record.iStartTime);
    record.iCollapsedTime=static_cast<int>(static_cast<unsigned int>(record.iCollapsedTime)+elapsed);
    record.iCallingCount=static_cast<int>(static_cast<unsigned int>(record.iCallingCount)+1U);
    m_iAccumDepth=static_cast<int>(static_cast<unsigned int>(m_iAccumDepth)-1U);
}
// Caller-inline evidence:080cde25..080cde5e, old filename profile.txt/mode w.
inline void CProfiler::Log(const char* filename) {
    FILE* stream=std::fopen(filename,"w");
    if(!stream) return;
    Print(stream); std::fclose(stream);
}
#endif

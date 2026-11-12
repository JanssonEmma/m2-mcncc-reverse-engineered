#include "../../game/src/profiler.h"
#include <cassert>
#include <cstring>
#include <cstdio>
static DWORD tick;
static int clock_calls;
extern "C" DWORD get_dword_time() { ++clock_calls; return tick; }
class Probe : public CProfiler {
public:
    void CheckInitial() { assert(!m_iAccumDepth && !m_ProfileStackDataCount && !m_iCallStep && m_vec_Accum.empty() && m_ProfileAccumDataMap.empty()); }
    int Depth() const { return m_iAccumDepth; }
    void Depth(int value) { m_iAccumDepth=value; }
    std::size_t Count() const { return m_vec_Accum.size(); }
    TProfileAccumData* At(unsigned i) { return m_vec_Accum[i]; }
    void Stack() {
        m_ProfileStackDataCount=2;
        m_ProfileStackDatas[0].iCallStep=0; m_ProfileStackDatas[0].strName="root";
        m_ProfileStackDatas[0].iStartTime=10; m_ProfileStackDatas[0].iEndTime=35;
        m_ProfileStackDatas[1].iCallStep=2; m_ProfileStackDatas[1].strName="child";
        m_ProfileStackDatas[1].iStartTime=100; m_ProfileStackDatas[1].iEndTime=93;
    }
};
static std::string Report(CProfiler& profiler) {
    FILE* stream=std::tmpfile(); assert(stream); profiler.Print(stream); std::rewind(stream);
    char bytes[4096]; std::size_t size=std::fread(bytes,1,sizeof bytes,stream); std::fclose(stream);
    return std::string(bytes,size);
}
int main() {
    assert(!CProfiler::instance_ptr());
    {
        Probe profiler; profiler.CheckInitial(); assert(CProfiler::instance_ptr()==&profiler);
        assert(Report(profiler)=="Name                                 Call/Load       Ratio\n");
        tick=100; profiler.PushAccum("root");
        assert(profiler.Count()==1 && profiler.Depth()==1 && profiler.At(0)->iDepth==0 && profiler.At(0)->iStartTime==100);
        assert(!profiler.At(0)->iCallingCount && !profiler.At(0)->iCollapsedTime);
        tick=200; profiler.PushAccum("child");
        assert(profiler.Count()==2 && profiler.Depth()==2 && profiler.At(1)->iDepth==1);
        tick=250; profiler.PopAccum("child");
        assert(profiler.Depth()==1 && profiler.At(1)->iCollapsedTime==50 && profiler.At(1)->iCallingCount==1);
        tick=300; profiler.PopAccum("root");
        assert(profiler.Depth()==0 && profiler.At(0)->iCollapsedTime==200 && profiler.At(0)->iCallingCount==1);
        const int calls=clock_calls; profiler.PopAccum("missing"); assert(clock_calls==calls && profiler.Depth()==0);
        CProfiler::TProfileAccumData* first=profiler.At(0);
        first->iCallingCount=3; first->iCollapsedTime=10;
        profiler.Depth(9); tick=0x80000001UL; profiler.PushAccum("root");
        assert(profiler.Count()==2 && profiler.Depth()==10 && profiler.At(0)==first);
        assert(first->iDepth==0 && static_cast<unsigned int>(first->iStartTime)==0x80000001U);
        assert(first->iCallingCount==3 && first->iCollapsedTime==10);
        profiler.At(1)->iCollapsedTime=0; profiler.At(1)->iCallingCount=0;
        profiler.Stack(); std::string output=Report(profiler);
        assert(output.find("root                    : 25\n")==0);
        assert(output.find("\t\tchild                   : -7\n")!=std::string::npos);
        assert(output.find("3/10         3.33\n")!=std::string::npos);
        assert(output.find("     child")!=std::string::npos && output.find("0/0          0.00\n")!=std::string::npos);
        profiler.Initialize();
        assert(profiler.Depth()==0 && profiler.Count()==2 && first->iCallingCount==3);
        tick=77; profiler.PushAccum("third"); assert(profiler.Count()==3 && profiler.At(2)->strName=="third" && profiler.At(2)->iDepth==0);
        profiler.Depth(0); tick=100; profiler.PushAccum("recursive"); tick=110; profiler.PushAccum("recursive");
        tick=120; profiler.PopAccum("recursive"); tick=130; profiler.PopAccum("recursive");
        assert(profiler.Depth()==0 && profiler.At(3)->iCallingCount==2 && profiler.At(3)->iCollapsedTime==30);
        profiler.Log("");
        const char* filename="profiler_regression_output.txt"; profiler.Log(filename);
        FILE* file=std::fopen(filename,"r"); assert(file);
        char bytes[4096]; std::size_t size=std::fread(bytes,1,sizeof bytes,file); std::fclose(file);
        assert(std::string(bytes,size)==Report(profiler)); assert(std::remove(filename)==0);
    }
    assert(!CProfiler::instance_ptr());
    std::puts("profiler passed: nested accumulation, stable insertion order, repeated names and exact report formatting");
}

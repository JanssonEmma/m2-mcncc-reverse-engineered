#include <string>
#include <vector>
#include <ext/hash_map>
#define protected public
#include "../../game/src/profiler.h"
#undef protected
#include <cstddef>
#define ABI(name,expression) typedef char name[(expression)?1:-1]
ABI(stack_record,sizeof(CProfiler::TProfileStackData)==16);
ABI(accum_record,sizeof(CProfiler::TProfileAccumData)==20);
ABI(profiler_size,sizeof(CProfiler)==1072);
ABI(stack_count,offsetof(CProfiler,m_ProfileStackDataCount)==4);
ABI(stack_records,offsetof(CProfiler,m_ProfileStackDatas)==8);
ABI(accum_map,offsetof(CProfiler,m_ProfileAccumDataMap)==1032);
ABI(accum_vector,offsetof(CProfiler,m_vec_Accum)==1052);
ABI(accum_depth,offsetof(CProfiler,m_iAccumDepth)==1064);
ABI(call_step,offsetof(CProfiler,m_iCallStep)==1068);

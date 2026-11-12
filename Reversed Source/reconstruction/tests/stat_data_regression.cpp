#include "../../game/src/constants.h"
#include "../../game/src/char.h"
#include "stat_data_golden.h"
#include <cassert>
#include <cstring>
#include <cstdio>
int main() {
    assert(sizeof(JobInitialPoints)==sizeof(original_JobInitialPoints));
    assert(sizeof(aApplyInfo)==sizeof(original_aApplyInfo));
    assert(!std::memcmp(JobInitialPoints,original_JobInitialPoints,sizeof(JobInitialPoints)));
    assert(!std::memcmp(aApplyInfo,original_aApplyInfo,sizeof(aApplyInfo)));
    for(unsigned i=0;i<sizeof(original_enum_values)/sizeof(*original_enum_values);++i)
        assert(original_enum_values[i].actual==original_enum_values[i].expected);
    int original=JobInitialPoints[JOB_SHAMAN].hp_per_ht;
    JobInitialPoints[JOB_SHAMAN].hp_per_ht=-7;
    assert(JobInitialPoints[JOB_SHAMAN].hp_per_ht==-7);
    JobInitialPoints[JOB_SHAMAN].hp_per_ht=original;
    assert(!std::memcmp(JobInitialPoints,original_JobInitialPoints,sizeof(JobInitialPoints)));
    std::puts("old stat data passed: original338 table bytes,255 enumerator values and mutable job data");
}

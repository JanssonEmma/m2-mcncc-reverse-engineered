// Staged ABI check. Do not build until candidate fragments are integrated.
#include "../../game/src/constants.h"
#include "../../game/src/char.h"
#include <cstddef>
#define STAT_ASSERT(name,expr) typedef char stat_assert_##name[(expr)?1:-1]
STAT_ASSERT(int32,sizeof(int)==4);
STAT_ASSERT(long32,sizeof(long)==4);
STAT_ASSERT(job64,sizeof(TJobInitialPoints)==64);
STAT_ASSERT(apply1,sizeof(TApplyInfo)==1);
STAT_ASSERT(jobcount,JOB_MAX_NUM==4);
STAT_ASSERT(applycount,MAX_APPLY_NUM==82);
STAT_ASSERT(pointcount,POINT_MAX_NUM==255);
STAT_ASSERT(job_st,offsetof(TJobInitialPoints,st)==0);
STAT_ASSERT(job_ht,offsetof(TJobInitialPoints,ht)==4);
STAT_ASSERT(job_dx,offsetof(TJobInitialPoints,dx)==8);
STAT_ASSERT(job_iq,offsetof(TJobInitialPoints,iq)==12);
STAT_ASSERT(job_max_hp,offsetof(TJobInitialPoints,max_hp)==16);
STAT_ASSERT(job_max_sp,offsetof(TJobInitialPoints,max_sp)==20);
STAT_ASSERT(job_hp_per_ht,offsetof(TJobInitialPoints,hp_per_ht)==24);
STAT_ASSERT(job_sp_per_iq,offsetof(TJobInitialPoints,sp_per_iq)==28);
STAT_ASSERT(job_hp_per_lv_begin,offsetof(TJobInitialPoints,hp_per_lv_begin)==32);
STAT_ASSERT(job_hp_per_lv_end,offsetof(TJobInitialPoints,hp_per_lv_end)==36);
STAT_ASSERT(job_sp_per_lv_begin,offsetof(TJobInitialPoints,sp_per_lv_begin)==40);
STAT_ASSERT(job_sp_per_lv_end,offsetof(TJobInitialPoints,sp_per_lv_end)==44);
STAT_ASSERT(job_max_stamina,offsetof(TJobInitialPoints,max_stamina)==48);
STAT_ASSERT(job_stamina_per_con,offsetof(TJobInitialPoints,stamina_per_con)==52);
STAT_ASSERT(job_stamina_per_lv_begin,offsetof(TJobInitialPoints,stamina_per_lv_begin)==56);
STAT_ASSERT(job_stamina_per_lv_end,offsetof(TJobInitialPoints,stamina_per_lv_end)==60);
STAT_ASSERT(apply_offset,offsetof(TApplyInfo,bPointType)==0);
STAT_ASSERT(job_table256,sizeof(JobInitialPoints)==256);
STAT_ASSERT(apply_table82,sizeof(aApplyInfo)==82);
STAT_ASSERT(character_point1056,sizeof(CHARACTER_POINT)==1056);
STAT_ASSERT(character_point_instant1688,sizeof(CHARACTER_POINT_INSTANT)==1688);

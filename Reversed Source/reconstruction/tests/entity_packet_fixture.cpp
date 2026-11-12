// Explicit unused descriptor boundary for isolated entity/name suites.
// Packet fanout is tested with real DESC::Packet in desc_control_regression.
#include "../../game/src/desc.h"
#include <cstdlib>
#include <cstring>
// Optional capture hooks used by the item lifecycle suite; absent hooks preserve abort boundaries.
void (*isolated_packet_hook)(const void*,int)=0;
void DESC::Packet(const void* data,int bytes) { if(isolated_packet_hook) isolated_packet_hook(data,bytes); else std::abort(); }
extern "C" void fw_assert_fail(const char* condition,const char* file,int line) { std::fprintf(stderr,"entity fixture assertion %s at %s:%d\n",condition,file,line); std::abort(); }

extern "C" void _sys_err(const char*,int,const char*,...) { std::abort(); }

extern "C" { void* (*thecore_memcpy)(void*,const void*,size_t)=std::memcpy; }

#include "../../game/src/event.h"
#include <ctime>
// Unused horse timer/clock boundary in isolated name/entity suites; real horse events have a separate integration suite.
void (*isolated_cancel_hook)(LPEVENT*)=0;
extern "C" void event_cancel(LPEVENT* handle) { if(isolated_cancel_hook) isolated_cancel_hook(handle); else if(*handle) std::abort(); }
extern "C" DWORD __AllocEventInfo(int,void**) { std::abort(); }
extern "C" LPEVENT event_create_ex(TEVENTFUNC,DWORD,long,char*,int) { std::abort(); }
extern "C" long event_time(LPEVENT) { std::abort(); }
extern "C" int MINMAX(int,int,int) { std::abort(); }
time_t get_global_time() { std::abort(); }
int passes_per_sec=25;
unsigned isolated_race_registration_logs=0;
unsigned isolated_monarch_logs=0;
unsigned isolated_item_packet_logs=0;
extern "C" void sys_log(int level,const char* format,...) {
    if(level==2 && (!std::strcmp(format,"Item::EncodeInsertPacket %s to %s") || !std::strcmp(format,"Item::EncodeRemovePacket %s to %s"))) { ++isolated_item_packet_logs; return; }
    if(level!=0) std::abort();
    if(!std::strcmp(format,"RegisterRaceNumMap %s %lu")) ++isolated_race_registration_logs;
    else if(!std::strcmp(format," Pulse %d cooltime %d , limit %d")) ++isolated_monarch_logs;
    else std::abort();
}

#include "../../libthecore/include/main.h"
static HEART isolated_fixture_heart={0,{0,0},{0,0},{0,0},25,0};
LPHEART thecore_heart=&isolated_fixture_heart;

extern "C" int MIN(int,int) { std::abort(); }
int test_server=0;
// Configuration input for the unused locale-loader path in isolated graphs.
// The dedicated loader integration exercises both auth states.
bool g_bAuthServer=false;
BYTE DESC::GetEmpire() { std::abort(); }

DWORD isolated_dword_clock=0;
unsigned isolated_pulse_reads=0;
DWORD isolated_dword_step=0;
unsigned isolated_dword_reads=0;
extern "C" DWORD get_dword_time() { ++isolated_dword_reads; DWORD value=isolated_dword_clock; isolated_dword_clock+=isolated_dword_step; return value; }
float isolated_float_clock=0;
unsigned isolated_float_reads=0;
extern "C" float get_float_time() { ++isolated_float_reads; return isolated_float_clock; }
extern "C" long double get_float_time_extended() { ++isolated_float_reads; return isolated_float_clock; }
extern "C" int thecore_pulse() { ++isolated_pulse_reads; return thecore_heart->pulse; }

// TEST ONLY fail-fast boundary; real shutdown is exercised in item_id_regression.
extern "C" void thecore_shutdown(void) { std::abort(); }

// TEST ONLY unused legacy event-registry boundary, never a successful result.
// Dedicated quest PC suite defines MCNCC_TEST_REAL_EVENTS and links actual event.cpp.
#ifndef MCNCC_TEST_REAL_EVENTS
extern "C" void* FindEventInfo(DWORD) { std::abort(); }
#endif

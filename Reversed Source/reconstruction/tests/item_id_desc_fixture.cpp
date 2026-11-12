// Explicit test-only application/subsystem boundaries. Never linked into production.
#include "desc_fixture.h"
#include "../../game/src/config.h"
#include <cstdlib>
extern "C" void fw_assert_fail(const char* condition, const char* file, int line)
{
    std::fprintf(stderr, "descriptor fixture assertion %s at %s:%d\n", condition, file, line);
    std::abort();
}

int desc_test_phase_calls = 0;


int desc_test_clock_calls = 0;
time_t get_global_time() { ++desc_test_clock_calls; return 1700000000; }

// Configuration storage remains a test boundary; bool declaration is reference-guided.
bool g_bTrafficProfileOn = false;
bool g_bAuthServer = false;

// Explicit test-only doubles for unrecovered application dispatch.
#include "../../game/src/input.h"
int test_server=0;
int CInputHandshake::Analyze(DESC*, BYTE, const char*) { std::abort(); }
int CInputLogin::Analyze(DESC*, BYTE, const char*) { std::abort(); }
int CInputMain::Analyze(DESC*, BYTE, const char*) { std::abort(); }
int CInputDead::Analyze(DESC*, BYTE, const char*) { std::abort(); }
int CInputAuth::Analyze(DESC*, BYTE, const char*) { std::abort(); }

#include "../../game/src/char.h"
#include "../../game/src/desc_manager.h"
#include "../../game/src/desc_client.h"
#include "../../game/src/event.h"
// Explicit subsystem boundaries while character disconnect and manager/client cleanup remain pending.
void (*desc_test_character_disconnect)(CHARACTER*)=0;
void (*desc_test_cancel_event)(LPEVENT*)=0;
void CHARACTER::Disconnect() { if(desc_test_character_disconnect) desc_test_character_disconnect(this); else std::abort(); }
#ifndef MCNCC_TEST_REAL_EVENTS
extern "C" void event_cancel(LPEVENT* event) {
    if(*event) { if(desc_test_cancel_event) desc_test_cancel_event(event); else std::abort(); }
}
#endif
DESC_MANAGER::~DESC_MANAGER() {}
int client_test_destroy_calls=0;
void CLIENT_DESC::Destroy() { ++client_test_destroy_calls; if(!m_lpOutputBuffer) m_lpOutputBuffer=buffer_new(0,1); if(!m_lpInputBuffer) m_lpInputBuffer=buffer_new(0,1); }
void CLIENT_DESC::SetPhase(int) { std::abort(); }

#ifndef MCNCC_TEST_REAL_EVENTS
extern "C" DWORD __AllocEventInfo(int,void**) { std::abort(); }
extern "C" LPEVENT event_create_ex(TEVENTFUNC,DWORD,long,char*,int) { std::abort(); }
extern "C" long event_time(LPEVENT) { std::abort(); }
#endif
int passes_per_sec=25;

#include "../../libthecore/include/main.h"
// ID suite links actual libthecore main.c globals/pulse/shutdown.





// TEST ONLY unused legacy event-registry boundary, never a successful result.
// Dedicated quest PC suite defines MCNCC_TEST_REAL_EVENTS and links actual event.cpp.
#ifndef MCNCC_TEST_REAL_EVENTS
extern "C" void* FindEventInfo(DWORD) { std::abort(); }
#endif

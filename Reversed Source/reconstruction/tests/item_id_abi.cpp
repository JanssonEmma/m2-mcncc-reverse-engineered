#include "../../game/src/item_manager.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
typedef char id_word32[sizeof(DWORD)==4?1:-1];
typedef char id_range8[sizeof(g_dwItemIDRange)==8?1:-1];
static DWORD (ITEM_MANAGER::*get_id)()=&ITEM_MANAGER::GetNewID;
static bool (ITEM_MANAGER::*set_id)(DWORD)=&ITEM_MANAGER::SetMaxItemID;
static DWORD (&range_type)[2]=g_dwItemIDRange;
static void (*shutdown_type)()=&thecore_shutdown;
static volatile int* shutdown_state_type=&shutdowned;

#include <map>
#include <string>
#include <vector>
#include "../../game/src/questpc.h"
void BuildCancelLoadedAPI(){void(quest::PC::*build)()=&quest::PC::Build;void(quest::PC::*cancel)()=&quest::PC::CancelRunning;void(quest::PC::*loaded)()=&quest::PC::SetLoaded;bool(quest::PC::*isLoaded)()=&quest::PC::IsLoaded;(void)build;(void)cancel;(void)loaded;(void)isLoaded;}
typedef char build_int32[sizeof(int)==4?1:-1];
typedef char build_dword32[sizeof(DWORD)==4?1:-1];
// Full STL-bearing records retained. Old whole-class byte sizes are not nativeSDK ABI gates.

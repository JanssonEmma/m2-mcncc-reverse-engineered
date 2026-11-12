// TEST ONLY external strcasecmp observer around full actual original skill.cpp.
#include "../../game/src/skill.h"
#include "../../common/tables.h"
#include "../../libthecore/include/log.h"
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <map>
#include <utility>
extern "C" int skill_factory_strcasecmp(const char*,const char*);
#ifdef strcasecmp
#undef strcasecmp
#endif
#define strcasecmp skill_factory_strcasecmp
#include "../../game/src/skill.cpp"
#undef strcasecmp

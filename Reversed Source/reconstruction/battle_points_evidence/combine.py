from pathlib import Path
p=Path('work/battle_points')
inc='''// Full target-backed battle dependency candidate; do not substitute PointChange.
#include "char.h"
#include "item.h"
#include "mob_manager.h"
#include "questmanager.h"
#include "constants.h"
#include "config.h"
#include "locale_service.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
'''
p.joinpath('char-battle-complete.cpp.candidate').write_text(inc+p.joinpath('char-helper-methods.cpp.candidate').read_text()+'\n'+p.joinpath('compute-method.cpp.candidate').read_text())

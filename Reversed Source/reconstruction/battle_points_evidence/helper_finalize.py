# Resolve portable paths from the repository root, regardless of current working directory.
import sys as _portable_sys
from pathlib import Path as _portable_Path
for _portable_parent in (_portable_Path(__file__).resolve().parent, *_portable_Path(__file__).resolve().parents):
    if (_portable_parent / 'project_paths.py').is_file():
        _portable_sys.path.insert(0, str(_portable_parent))
        break
else:
    raise RuntimeError('project_paths.py not found in repository ancestors')
from project_paths import resolve_path as _resolve_portable_path
from pathlib import Path
p=Path('work/battle_points/helper_regression.cpp');s=p.read_text();a=s.index('    for(unsigned b=0;b<5;++b)for(unsigned t=0;t<4;++t)for(unsigned f=1;f<5;++f){');b=s.index('    // Missing mob',a)
s=s[:a]+'''    for(unsigned point=12;point<=15;++point)for(unsigned b=0;b<5;++b)for(unsigned t=0;t<4;++t)for(unsigned f=1;f<5;++f){
        ch.m_pointsInstant.points[point]=bases[b];
        switch(point){case12:mob.m_table.bStr=static_cast<BYTE>(stats[t]);break;case13:mob.m_table.bCon=static_cast<BYTE>(stats[t]);break;case14:mob.m_table.bDex=static_cast<BYTE>(stats[t]);break;default:mob.m_table.bInt=static_cast<BYTE>(stats[t]);break;}
        quests.m_mapEventFlag["poly"]=flags[f];assert(ch.GetPolymorphPoint(static_cast<BYTE>(point))==Oracle(bases[b],stats[t],flags[f]));
    }
'''+s[b:]
s=s.replace('case12:','case 12:').replace('case13:','case 13:').replace('case14:','case 14:')
s=s.replace('ch.m_dwPolymorphRace=0;assert(ch.GetPolymorphPoint(95)==333);ch.m_dwPolymorphRace=1001;ch.m_bPolyMaintainStat=true;assert(ch.GetPolymorphPoint(95)==333);','ch.m_dwPolymorphRace=0;assert(ch.GetPolymorphPoint(95)==333&&ch.GetPolymorphPoint(12)==ch.GetPoint(12));ch.m_dwPolymorphRace=1001;ch.m_bPolyMaintainStat=true;assert(ch.GetPolymorphPoint(95)==333&&ch.GetPolymorphPoint(12)==ch.GetPoint(12));')
p.write_text(s)
p=Path('work/battle_points');skill=p.joinpath('helper-char-skill.cpp.inc').read_text();p.joinpath('helper-char-skill.cpp.candidate').write_text('#include "char.h"\n#include "../../libthecore/include/utils.h"\n#include "../../libthecore/include/log.h"\n'+skill)
m=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\Makefile')).read_text();i=m.index('target_walking_regression$(EXEEXT):');block=m[i:m.index('\n\n',i)].replace('target_walking_regression','helper_regression').replace('$(TARGET_WALKING_SOURCES)','$(POLY_HELPER_SOURCES)').replace('$(TARGET_WALKING_OBJECTS)','$(POLY_HELPER_OBJECTS)')
preamble='''# Shared graph already includes real CHAR/CMob/quest/locale/point modules.
POLY_HELPER_SOURCES = $(filter-out desc_fixture.cpp ../../game/src/char_skill.cpp,$(TARGET_WALKING_SOURCES)) helper_desc_fixture.cpp ../../game/src/char_skill.cpp
POLY_HELPER_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) helper_desc_output_syscalls.o

'''
recipe='''
helper_desc_output_syscalls.o: helper_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c helper_desc_output_syscalls.c -o $@
'''
p.joinpath('Makefile.helper.addition.candidate').write_text(preamble+block+'\n'+recipe)

from pathlib import Path
p=Path('work/battle_points');s=p.joinpath('char-helper-methods.cpp.candidate').read_text();a=s.index('//08073560:');b=s.index('//08135a20:')
p.joinpath('helper-char-skill.cpp.inc').write_text('// Original placement char_skill.cpp2611, STB_GLOBAL and NOT inline.\n// New partial game/src/char_skill.cpp needs char.h, coreutils.h/log.h.\n'+s[a:b])
s=s[:a]+s[b:];p.joinpath('helper-char.cpp.inc').write_text('// Original concrete bodies all in char.cpp; append after required includes.\n'+s)
p.joinpath('helper-declarations.inc').write_text('long GetPolymorphPoint(BYTE type);\nint GetPolymorphPower();\nint GetSkillLevel(DWORD dwVnum);\nconst TMobTable& GetMobTable();\n')

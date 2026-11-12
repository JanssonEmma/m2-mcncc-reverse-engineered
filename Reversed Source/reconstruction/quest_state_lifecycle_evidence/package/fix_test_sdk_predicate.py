from pathlib import Path
p=Path('work/quest_state_lifecycle/quest_state_lifecycle_regression.cpp');s=p.read_text(encoding='utf-8-sig');assert 'lua_isthread(L,-1)!=0' in s;s=s.replace('lua_isthread(L,-1)!=0','lua_type(L,-1)==LUA_TTHREAD');p.write_text(s,encoding='utf-8')

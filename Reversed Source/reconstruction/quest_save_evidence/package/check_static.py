from pathlib import Path
import ast,json,hashlib
p=Path('work/pc_quest_save')
q=p/'prepare_integration.py';s=q.read_text(encoding='utf-8-sig').replace("'../../libthecore/src/buffer.c'","'buffer_manager_core.o'");q.write_text(s,encoding='utf-8')
for q in p.glob('*.py'):ast.parse(q.read_text(encoding='utf-8-sig'),filename=str(q))
print('Python AST valid; no C++ compile performed')

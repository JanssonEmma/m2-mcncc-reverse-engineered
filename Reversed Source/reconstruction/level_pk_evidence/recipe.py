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
p=Path('work/level_pk');m=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\Makefile')).read_text();i=m.index('character_update_regression$(EXEEXT):');block=m[i:m.index('\n\n',i)].replace('character_update_regression','level_pk_regression').replace('desc_output_syscalls.o','level_pk_desc_output_syscalls.o')
recipe='''
level_pk_desc_output_syscalls.o: level_pk_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c level_pk_desc_output_syscalls.c -o $@
'''
(p/'Makefile.addition.candidate').write_text(block+'\n'+recipe)

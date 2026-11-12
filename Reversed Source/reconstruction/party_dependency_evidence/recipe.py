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
p=Path('work/party_dependency/party_dependency_regression.cpp');s=p.read_text().replace('Member(party,10,&source,231)','Member(party,1234,&source,231)').replace('Member(party,40,&other,3)','Member(party,2000,&other,3)');p.write_text(s)
m=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\Makefile')).read_text();start=m.index('item_accessory_setters_regression$(EXEEXT):') if 'item_accessory_setters_regression$(EXEEXT):' in m else m.index('item_socket_expiry_regression$(EXEEXT):');block=m[start:m.index('\n\n',start)]
block=block.replace('item_accessory_setters_regression','party_dependency_regression').replace('item_socket_expiry_regression','party_dependency_regression').replace('accessory_desc_output_syscalls.o','party_desc_output_syscalls.o').replace('desc_output_syscalls.o','party_desc_output_syscalls.o')
block=block.replace('party_party_desc_output_syscalls.o','party_desc_output_syscalls.o')
# Avoid duplicate party sources if parent adds them to the shared closures.
block=block.replace('$(DESC_PACKET_SOURCES)','$(PARTY_PACKET_SOURCES)')
block=block.replace('$(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)\n','$(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)\n')
preamble='''# Add real party.cpp and the explicit test-only full-data virtual/lifecycle
# boundary to every legacy character executable's shared closure as described
# in INTEGRATE_PARTY_TEST.txt. The new target also works before that migration.
PARTY_PACKET_SOURCES = $(filter-out ../../game/src/party.cpp party_lifecycle_fixture.cpp,$(DESC_PACKET_SOURCES)) ../../game/src/party.cpp party_lifecycle_fixture.cpp

'''
recipe='''
party_desc_output_syscalls.o: party_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c party_desc_output_syscalls.c -o $@
'''
Path('work/party_dependency/Makefile.party-test.addition.candidate').write_text(preamble+block+'\n'+recipe)

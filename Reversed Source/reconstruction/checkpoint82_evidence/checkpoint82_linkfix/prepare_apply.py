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
import hashlib,json
from pathlib import Path
p=Path(__file__).resolve().parent;r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));base=p/'parent_apply';payload=base/'payload';changes=[]
def sha(b):return hashlib.sha256(b).hexdigest()
def put(rel,data):
 old=(r/rel).read_bytes() if (r/rel).exists() else None
 if isinstance(data,str):
  if old is not None and b'\r\n' in old:data=data.replace('\r\n','\n').replace('\n','\r\n')
  data=data.encode()
 dest=payload/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
 changes.append(dict(target=rel,expected_sha256=sha(old) if old is not None else None,desired_sha256=sha(data),payload=str(dest.relative_to(base))))
s=(r/'reconstruction/tests/Makefile').read_text();before=s
names=['desc_output_unit','desc_output_integration','desc_packet_integration','input_regression','desc_input_integration','desc_flush_regression','desc_control_regression'];rng={'desc_output_unit','input_regression','desc_flush_regression'}
lines=s.splitlines(keepends=True)
for n in names:
 hits=[i for i,l in enumerate(lines) if l.startswith(n+'$(EXEEXT):')];assert len(hits)==1
 i=hits[0];assert lines[i+1].startswith('\t$(CXX) ')
 assert 'item_socket_config.o' not in lines[i]+lines[i+1]
 extra=' item_socket_config.o'
 if n in rng:
  assert 'desc_utils.o' not in lines[i]+lines[i+1]
  extra+=' desc_rng_only.o desc_rng_os_boundary.o'
 else:assert 'desc_utils.o' in lines[i]+lines[i+1]
 lines[i]=lines[i].rstrip('\n')+extra+'\n'
 assert ' $(LDFLAGS)' in lines[i+1]
 lines[i+1]=lines[i+1].replace(' $(LDFLAGS)',extra+' $(LDFLAGS)',1)
s=''.join(lines)
assert [l for l in s.splitlines() if l.startswith('RUNTIME_TESTS = ')]==[l for l in before.splitlines() if l.startswith('RUNTIME_TESTS = ')]
assert len(next(l for l in s.splitlines() if l.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split())==88
s+='''
# TEST ONLY exact COFF selection from complete actual utils.c (no objcopy retarget).
RNG_PROJECTION_PYTHON ?= USER_HOME/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/python.exe
desc_rng_full.o: ../../libthecore/src/utils.c ../../libthecore/include/utils.h ../../libthecore/include/memcpy.h posix_shim/sys/time.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -fno-lto -ffunction-sections -fdata-sections -c ../../libthecore/src/utils.c -o $@
desc_rng_only.o: desc_rng_full.o project_rng.py
	$(RNG_PROJECTION_PYTHON) project_rng.py desc_rng_full.o $@ desc_rng_projection.proof.json --source ../../libthecore/src/utils.c
desc_rng_os_boundary.o: utils_link_stubs.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fno-lto -UNDEBUG -c utils_link_stubs.c -o $@
# Standalone projection-call integrity gate; deliberately outside the 88-test registry.
rng_projection_smoke$(EXEEXT): rng_projection_smoke.c desc_rng_only.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -fno-lto -UNDEBUG rng_projection_smoke.c desc_rng_only.o $(LDFLAGS) -o $@
.PHONY: clean-desc-rng-projection
clean: clean-desc-rng-projection
clean-desc-rng-projection:
	$(RM) desc_rng_full.o desc_rng_only.o desc_rng_os_boundary.o desc_rng_projection.proof.json rng_projection_smoke$(EXEEXT)
'''
put('reconstruction/tests/Makefile',s)
for f in ['project_rng.py','rng_projection_smoke.c']:put('reconstruction/tests/'+f,(p/f).read_bytes())
(base/'plan.json').write_text(json.dumps(dict(root=str(r),changes=changes,note='TEST ONLY seven exact real config links; three exact actual RNG object links+existing fatal OS boundary. Registry88 unchanged. No production edits.'),indent=2))
print('Prepared',len(changes),'files, seven config links and three real RNG links; registry88 unchanged.')

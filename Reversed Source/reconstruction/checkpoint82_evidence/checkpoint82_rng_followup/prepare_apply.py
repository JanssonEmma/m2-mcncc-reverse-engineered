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
p=Path(__file__).resolve().parent;r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));base=p/'parent_apply';payload=base/'payload';rel='reconstruction/tests/Makefile';old=(r/rel).read_bytes();s=old.decode();before=s
lines=s.splitlines(keepends=True);changed=[];audit=[]
for i,l in enumerate(lines):
 if not l.startswith('\t$(CXX)'):continue
 if not any(x in l for x in ('movement_duration_impl.o','game_time_impl.o')):continue
 target=lines[i-1].split(':',1)[0]
 assert target.endswith('$(EXEEXT)')
 provider='existing_projection' if 'desc_rng_only.o' in l else 'existing_core' if 'desc_utils.o' in l else None
 # ITEM_SOCKET_OBJECTS contains SQL/config ONLY, not a real RNG provider.
 if provider is None:
  extra=' desc_rng_only.o desc_rng_os_boundary.o';assert ' $(LDFLAGS)' in l
  lines[i-1]=lines[i-1].rstrip('\r\n')+extra+'\r\n'
  lines[i]=l.replace(' $(LDFLAGS)',extra+' $(LDFLAGS)',1);changed.append(target);provider='added_actual_projection'
 audit.append(dict(target=target,provider=provider,item_socket_objects='ITEM_SOCKET_OBJECTS' in l))
s=''.join(lines)
assert [x for x in s.splitlines() if x.startswith('RUNTIME_TESTS = ')]==[x for x in before.splitlines() if x.startswith('RUNTIME_TESTS = ')]
assert len(changed)==7,changed
# All indirect shared graph object groups already contain complete desc_utils.o;
# bare DESC_PACKET_OBJECTS recipes were closed by the first guarded package.
for i,l in enumerate(lines):
 if l.startswith('\t$(CXX)') and '$(DESC_PACKET_OBJECTS)' in l:
  assert 'desc_utils.o' in l or 'desc_rng_only.o' in l,lines[i-1]
data=s.encode();dest=payload/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
sha=lambda b:hashlib.sha256(b).hexdigest()
(base/'plan.json').write_text(json.dumps(dict(root=str(r),changes=[dict(target=rel,expected_sha256=sha(old),desired_sha256=sha(data),payload=str(dest.relative_to(base)))],note='TEST ONLY seven further actual RNG links; existing projected real body+fatal external OS boundary; registry88 unchanged.'),indent=2))
(p/'audit.json').write_text(json.dumps(dict(changed=changed,direct_nonLTO_utils_graphs=audit,registry_unchanged=True),indent=2))
print(changed)

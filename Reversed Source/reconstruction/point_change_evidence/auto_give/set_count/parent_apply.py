# Parent-authorized integration only. Default validates and prints; --apply writes.
from pathlib import Path
import argparse,hashlib,json
p=Path(__file__).resolve().parent
args=argparse.ArgumentParser();args.add_argument('--root',required=True);args.add_argument('--apply',action='store_true');a=args.parse_args();root=Path(a.root).resolve();m=json.loads(p.joinpath('apply-guards.json').read_text())
blobs={}
for name,want in m['files'].items():
 path=root.joinpath(name);raw=path.read_bytes()
 if hashlib.sha256(raw).hexdigest()!=want:raise SystemExit('STALE source guard: '+name)
 blobs[name]=raw
candidate=p.joinpath('quickslot-chain.cpp.candidate').read_bytes()
if hashlib.sha256(candidate).hexdigest()!=m['candidate_sha256']:raise SystemExit('Candidate guard failed')
h=blobs['game/src/char.h'];anchor=b'    bool DelQuickslot(BYTE pos);'
if h.count(anchor)!=1 or b'void SyncQuickslot(' in h or b'void ChainQuickslotItem(' in h:raise SystemExit('Header anchor/existing API guard failed')
nl=b'\r\n' if b'\r\n' in h else b'\n'
h=h.replace(anchor,anchor+nl+b'    void SyncQuickslot(BYTE type,BYTE oldPos,BYTE newPos);'+nl+b'    void ChainQuickslotItem(LPITEM item,BYTE type,BYTE oldPos);')
s=blobs['game/src/char_quickslot.cpp'];anchor=b'#include "char.h"'
if s.count(anchor)!=1 or b'CHARACTER::SyncQuickslot(' in s or b'CHARACTER::ChainQuickslotItem(' in s:raise SystemExit('Source anchor/existing body guard failed')
nl=b'\r\n' if b'\r\n' in s else b'\n';s=s.replace(anchor,anchor+nl+b'#include "item.h"')
body=candidate.decode('utf-8-sig').replace('\r\n','\n').strip().replace('\n',nl.decode()).encode('utf8');s=s.rstrip()+nl+nl+body+nl
changes={'game/src/char.h':h,'game/src/char_quickslot.cpp':s}
# All guards completed before first write. No SetCount/removal/destruction integration.
if a.apply:
 for name,raw in changes.items():root.joinpath(name).write_bytes(raw)
print(json.dumps({'mode':'applied' if a.apply else 'validated_only','files':{n:hashlib.sha256(b).hexdigest() for n,b in changes.items()}},indent=2))

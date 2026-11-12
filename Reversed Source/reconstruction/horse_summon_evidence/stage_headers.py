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
import json,re
p=Path(__file__).parent;src=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\game\\src'))
methods=[
('HorseSummon','void HorseSummon(bool summon,bool fromFar);'),
('SetRider','void SetRider(LPCHARACTER rider);'),
('SetRotation','void SetRotation(float rotation);'),
('SetEmpire','void SetEmpire(BYTE empire);'),
('GetEmpire','BYTE GetEmpire() { return m_bEmpire; }'),
('GetVictim','LPCHARACTER GetVictim();'),
('Show','bool Show(long map,long x,long y,long z,bool spawnMotion);'),
('Goto','bool Goto(long x,long y);'),
('SendMovePacket','void SendMovePacket(BYTE function,BYTE argument,DWORD x,DWORD y,DWORD duration,DWORD time,int rotation);'),
('SetProto','void SetProto(const CMob* mob);'),
('StartAffectEvent','void StartAffectEvent();'),
('MonsterChat','void MonsterChat(BYTE type);')]
h=src.joinpath('char.h').read_text();adds=[decl for name,decl in methods if not re.search(r'\b'+name+r'\s*\(',h)]
p.joinpath('candidate-char.h').write_text(h.replace('    virtual ~CHARACTER();','    virtual ~CHARACTER();\n    '+'\n    '.join(adds)))
p.joinpath('char-api-additions.inc').write_text('\n'.join(adds)+'\n')
h=src.joinpath('guild.h').read_text()
assert 'GetMasterPID(' not in h
p.joinpath('candidate-guild.h').write_text(h.replace('    DWORD GetID()', '    DWORD GetMasterPID() { return m_data.master_pid; }\n    DWORD GetID()'))
p.joinpath('guild-api-additions.inc').write_text('DWORD GetMasterPID() { return m_data.master_pid; }\n')
h=src.joinpath('char_manager.h').read_text()
assert 'SpawnMob(' not in h
decl='LPCHARACTER SpawnMob(DWORD vnum,long map,long x,long y,long z,bool spawnMotion,int rotation,bool show);'
p.joinpath('candidate-char_manager.h').write_text(h.replace('    DWORD AllocVID();','    DWORD AllocVID();\n    '+decl))
p.joinpath('char-manager-api-additions.inc').write_text(decl+'\n')
packet=r"""
// Complete old packed packet records, target dead5/move24.
typedef struct packet_dead { BYTE header;DWORD vid; } TPacketGCDead;
typedef struct packet_move {
    BYTE bHeader,bFunc,bArg,bRot;
    DWORD dwVID;
    long lX,lY;
    DWORD dwTime,dwDuration;
} TPacketGCMove;
"""
h=src.joinpath('packet.h').read_text();assert 'packet_move' not in h and 'packet_dead' not in h
p.joinpath('candidate-packet.h').write_text(h.replace('#pragma pack(pop)',packet+'\n#pragma pack(pop)'))
p.joinpath('packet-record-additions.inc').write_text(packet)
p.joinpath('encode-move-api.inc').write_text('void EncodeMovePacket(TPacketGCMove&,DWORD,BYTE,BYTE,DWORD,DWORD,DWORD,DWORD,BYTE);\n')
# Old vtable addresspoint, not a guessed C++ source declaration order.
tables=json.loads(p.parent.joinpath('vtables.json').read_text());table=next(t for t in tables if t['symbol']=='_ZTV9CHARACTER')
entries=[]
for site,slot,name in [('0816da97',14,'CHARACTER::GetMyHorseVnum'),('0816d8eb',18,'CHARACTER::ClearHorseInfo'),('0816d8fd',17,'CHARACTER::SendHorseInfo')]:
 entry=table['words'][slot+2];entries.append({'callsite':site,'primary_vptr_slot':slot,'entry':entry,'resolved_method':name})
p.joinpath('virtual-call-resolution.json').write_text(json.dumps({'vtable_symbol':table['symbol'],'addresspoint':'08226d08','calls':entries,'show_indirect':'Show primary slot2=>CHARACTER::EncodeInsertPacket08128300; preserve virtual dispatch'},indent=2))
for f in p.glob('*-decompile.json'):
 d=json.loads(f.read_text());assert not d.get('truncated');f.with_suffix('.c').write_text(d['code'])
# Copy original full HorseSummon body/callgraph for self-contained evidence.
for name in ('0816d940-decompile.json','0816d940-decompile.c','0816d940-calls.json'):
 p.joinpath(name).write_bytes(p.parent.joinpath(name).read_bytes())
print('Full existing headers preserved with additive APIs; dead5/move24 records and exact virtual slots staged.')

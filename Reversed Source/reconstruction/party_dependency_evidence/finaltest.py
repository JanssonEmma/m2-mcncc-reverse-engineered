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
p=Path('work/party_dependency/party_dependency_regression.cpp');s=p.read_text().replace('assert(party.m_iLeadership==0&&party.m_iCountNearPartyMember==0&&party.m_dwPartyHealTime==party.m_dwPartyStartTime+1);','assert(party.m_iLeadership==0&&party.m_iCountNearPartyMember==0&&party.m_dwPartyHealTime==party.m_dwPartyStartTime+1);\n        assert(party.m_iExpDistributionMode==0&&!party.m_pkChrExpCentralize&&!party.m_pkChrLeader&&!party.m_pkDungeon&&!party.m_bPCParty);\n        assert(!party.m_bPartyHealReady&&!party.m_bCanUsePartyHeal&&party.m_iLongTimeExpBonus==0&&party.m_iExpBonus==0&&party.m_iAttBonus==0&&party.m_iDefBonus==0);')
p.write_text(s)
m=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\Makefile')).read_text();lines=m.splitlines();found=[]
for i,l in enumerate(lines):
 if l and not l.startswith(('\t','#')) and ':' in l and any(k in l for k in ['../../game/src/char.cpp','$(DESC_PACKET_SOURCES)']):found.append(str(i+1)+': '+l.split(':')[0])
Path('work/party_dependency/character-link-closure.txt').write_text('All direct char.cpp / shared DESC_PACKET_SOURCES targets observed in current Makefile:\n'+'\n'.join(found)+'\nOther source aliases must also be audited when parent changes their composition.\n')

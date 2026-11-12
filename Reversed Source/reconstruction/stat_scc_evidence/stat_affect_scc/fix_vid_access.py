from pathlib import Path
for file in ['work/stat_affect_scc/closed_leaves/char_battle.cpp.additions','work/stat_affect_scc/stage.py']:
 p=Path(file);s=p.read_text();s=s.replace('packet.dwStartVID=m_vid.m_id;packet.dwEndVID=victim->m_vid.m_id;','packet.dwStartVID=static_cast<DWORD>(m_vid);packet.dwEndVID=static_cast<DWORD>(victim->m_vid);');p.write_text(s)

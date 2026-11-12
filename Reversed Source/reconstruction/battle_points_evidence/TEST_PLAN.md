# Integration test plan (no new application stubs)

Promote helpers independently if useful; full PC battle tests require ACTUAL PointChange SCC. No fake PointChange/SetPoint/CMobManager/GetWear/quest/skill method is permitted. Reuse full existing CHARACTER/CMob/CItem/descriptor lifecycle fixtures and real lookup/field modules; external diagnostic/log/transport observers only.

## Skill and power helpers

1. Full packed TPlayerSkill[255], populate bLevel0..255 at skill129 with distinct bMasterType/tNextRead sentinels. GetSkillLevel129 returns MIN(40,storedbyte), untouched unrelated fields. Null pointer returns0. Valid vnums0/254; invalid255/UINT_MAX returns0 and exactly _sys_err(GetSkillLevel,2614,originalformat,name,vnum) then sys_log0 sameformat. Existing desc_output_syscalls handler rejects this new expected diagnostic: add a separate narrow observer wrapper for the new executable, not a production diagnostic bypass. Old API signatures int(DWORD), nonconst must compile.
2. For each stored skill byte0..255, GetPolymorphPower reads aiPolymorphPowerByLevel[MIN(40,byte)]. Verify table against independent constants-elf golden. Particularly39->100,40/255->0. Null skills->table0=10. Normalserver ignores questpoly flag. test_server1/-1/INT_MIN: actual quest map poly flag0 falls back; nonzero values1,50,-50,INT_MIN,INT_MAX returned verbatim with no positivity clamp. Restore quest map/server state.

## Polymorph point helper

Use a REAL CMob in actual manager map with STR80,CON60,DEX40,INT100,wDef7. Full player level10/ST20/HT25/DX30/IQ40; distinguish mobSTR fromINT and playerST fromIQ.

- No polymorph or maintainStat=true: all supported and arbitrary point types return unmodified base GetPoint(type).
- Nonmaintain poly, skill39(power100), job0: ST100,HT85,DX70,IQ140. Job3 OR job2+skillgroup2: ST120 from PLAYERIQ40+MOBSTR80 (not mobINT). OtherSura group uses100. Unrelated point95 passes through raw95.
- Skill40(power0): job0 ST20; job3 ST40 (still IQ base), HT25. Nullskills(power10): ST28 ordinary, ST48 caster; HT31,DX34,IQ50.
- Quest override-50: ordinaryST-20,HT-5,DX10,IQ-10. Test odd mobstat*INT_MIN and INT_MAX to verify low32 wrapped multiplication BEFORE signed /100 and final wrapped addition. Use a Python big-int modulo32 independent golden, not the candidate helper copied into tests.
- Missingmob: return requestedGetPoint, but preserve Power call after manager lookup; this ordering is assembly-proved. Avoid fake helper instrumentation just to count an otherwise pure call. Unsupported type similarly performs real Power lookup before passthrough.

## Battle formulas: branch/state retention

Seed point20(clientdef)=777 before polymorph/NPC calls and all other derived points with sentinels. Verify only18,16,22,23 are replaced;20 remains777. Missingmob poly sets those four to0 and performs no PolyPoint/Power lookup. A foundmob computes below. Descriptor-null nonpoly requires a real m_pkMobData and uses its wDef; actor enum itself must not choose branch. Monsteractor with descriptor must execute PC formulas; playeractor without descriptor executes NPC formulas. No hardening for null NPC mobdata should be introduced.

Polymorph with fixture above, level10,mobdef7:
- skill39/job0 -> ATT220,DEF102,MAGICATT220,MAGICDEF102.
- skill39/job3 -> ATT260,DEF102; copies same to magic.
- skill40/job0 -> ATT60,DEF42; job3 ->ATT100,DEF42.
- maintainStat true ->ATT60,DEF42 regardlesscasterjob.
- override-50/job0 ->ATT-20,DEF12; raw negative stored, no clamp.
NPC nonpoly ->ATT60,DEF42 using ST/HT/mobdef and actual SetPoint. SetPoint's existing behavior remains real.

## PC goldens after actual SCC is integrated

Set all other modifiers/points to0 except level10/ST20/HT25/DX30/IQ40. Populate wear slots via real inventory/equipment paths, or existing complete fixture field setup before calling actual GetWear; no GetWear substitutes.

Armor records: body(type2/sub0) value1=10,value5=2 ->14; head(sub1)5,1 ->7; boots(sub4)3,0 ->3; shield(sub2)-5,-1 ->-7. Total17. Put huge-value wrist(sub3), neck/ear, weapon(type1), nullproto and null items elsewhere; all excluded. No IsEquipped filter should be added to the calculation. Slot18 counted; all19 slots traversed. Do not call GetWear outside targetvalidrange.

Set bonuspoints95=3,96=4,111=5,97=6,98=7. With all actual PointChange percent/modifier inputs zero, expected final derived points:

| Job | ATT | YMIR DEF | Other DEF | ClientDEF YMIR/Other | MAGICATT | MAGICDEF |
|0/2|63|61|56|0/5|106|73|
|1|69|61|56|0/5|106|73|
|3|76|61|56|0/5|106|73|

These goldens depend on full real PointChange having all unrelated modifiers zero; audit its actual code when integrating. ClientDEF is computed before later DEFbonus4+5, so stays0/5 rather than reflecting finaldef. ATTbonus3 occurs after horse scaling, which distinguishes folding/reordering. All ten actual PointChange calls false,false; do not create an application method observer to replace them. Assembly/direct-callgraph records exact call order for review.

With raw mountVnum nonzero, horseLevel30 (existing actual horse armor59): unmountedtotalArmor17 is floored59, even when IsHorseRiding=false. WarriorATT123, Ninja135, Shaman149; Sura skillgroup1ATT93, othergroup123. YMIR finalDEF103, other98, Client0/5, MAGICATT106,MAGICDEF94. Verify Before/After external sys_log values (base60->120 for Warrior,60->90 for Sura group1) before PointChange. A narrow external sys_log callback may change real horse level after Before; subsequent actual computation must read the new level, demonstrating original callback order without replacing app methods.

ForeignDEF HT1 and-1 truncates HT/1.25 toward0 (no ROUND); test HT6 ->4 and HT-6 ->-4. Negative armor oddvalues check /2 towardzero. LargeST/DX/IQ/armor/horseproduct use independent modulo32 goldens and negative division towardzero, normal/O2/strict32. Cached levels/HT defense reads happen before armor loop as retained in candidate; no moving stat/bonus getters across actual PointChange calls.

## Integration safety and link closure

No production changes/builds in this package. Existing complete CHARACTER/point records, CMobManager/CMob/TMobTable, CItem/TItemTable, THorseStat and skillrecords are retained. New raw getter and table declarations should be added only once. Full candidate can be appended to char.cpp; then every char.cpp executable needs actual PointChange SCC plus constants/locale/mob/quest closure. SeparateTU requires the complete candidate in one file (namespace helpers included) or a private inline header. Helpers may be promoted separately before the SCC; never add a fake PointChange merely to get a green helper executable.

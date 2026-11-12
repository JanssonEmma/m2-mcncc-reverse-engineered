# Parent follow-up after checkpoint73

Staged complete GetTable08193080 and RealNumber081930c0 bodies in item-lookup.cpp.candidate from full saved original assembly. NO production edit/build/runtime pass. Original DWARF declarations/access still pending before promotion. Candidate includes the existing complete item_manager.h, adds no class/field substitutes.

Assembly review: top vector.size(), bottom0; wrapped sum then arithmetic-right-shift1; load current.dwVnum before equality and signed interval test; unsigned item-vnum comparison; bounds update low32. GetTable checks signed negative then returns full record address. Original potentially one-past empty/above-max behavior retained, no hardening.

Safe-domain future tests: sorted prototypes10,20,30; each existing match and misses5/15/25; actual returned identity and no vector mutation. Do not execute empty/high-missing invalid accesses under a fabricated runtime safety claim. Original scalar/method qualifiers and class access need DWARF confirmation, then full-header syntax and meaningful actual source integration. No test was run here.

## Integration follow-up
Original DWARF lookup-api-dwarf.json confirms public nonconst TItemTable* GetTable(DWORD), protected nonconst int RealNumber(DWORD), complete record154/class172. Initial broad tests using all lengths1..41 were invalid as a safety proof: for singleton with query belowfirst, top1/mid0 then top-1 produces a pre-termination vector[-1] read. Although that normal run passed, its receipt is superseded; no safe-runtime proof is claimed for those inputs. Corrected regression uses verified lengths3/7/15/31, queries<=largest, plus3entry unsigned-boundary vector. Production body unchanged. Read-before-termination hazard includes lower-bound/singleton cases, not only empty/above-max.

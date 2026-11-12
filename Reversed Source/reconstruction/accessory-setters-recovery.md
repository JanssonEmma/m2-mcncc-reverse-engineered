# Accessory socket grade and time mutation

Target SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. SetAccessorySocketGrade08168850 and SetAccessorySocketDownGradeTime08168910 now use actual socket mutation/packet/save/devlog/audit and locale/chat code. Old inline getters read raw slots0,1,2 and return int. Newer reference clamps are not imported.

Grade clamps with MINMAX(0,requested,rawmax), completes SetSocket0, re-reads slot0 for the lifetime table, performs signed int /60 on test_server, then calls SetTime(DWORD). Time always executes real SetSocket2 and only then re-reads test_server and owner to resolve the exact CP949 locale key and emit type1 chat. Octal escapes preserve original bytes in an ASCII source file. Invalid grade indices retain original preconditions and are not exercised as valid inputs.

The four constants arrays are original ELF read-only data with confirmed symbol names, sizes and values. External const-int spelling and max enum name are reference-supported: exhaustive retained DWARF lookup did not recover those declaration spellings. Numeric maximum3 and table32-bit indexing are instruction-confirmed. This provenance is recorded in accessory-setters-README.md and companion metadata/assembly files.

The new regression uses a bounded operation vector to verify both full mutation sequences before chat, with128 clamp/debug/translation cases, unchanged values, non-accessory prototype, raw maximum7 with safe index, raw negative getter observations, DWORD extrema and absent owners. A reentrant external SQL-unlock observation invokes actual SetSockets, changing grade1 to2 between first mutation and the table lookup; the resulting second packet/time must use grade2. No production methods are replaced.

A separate descriptor syscall fixture accepts only the exact disabled-locale locale_find:51 diagnostic, asserting key/format/order after final SQL before chat. Other unexpected errors remain fatal. Existing shared descriptor fixtures are unchanged.

Normal focused integration passes; selected C++ application sources and four SQL/config/API objects also pass at O2. Updated item/constants sources pass strict32-bit C++03 syntax and method/table ABI gates. Full checkpoint64 completed exit0:61 exact runtime commands and check-abi passed. Original ELF/native DB/OS behavior and full accessory expiry/stat/equipment implementation are not proved by these tests.

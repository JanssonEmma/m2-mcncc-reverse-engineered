# Supplied Lua5.0.3 SDK dependency

These files are copied unchanged from REFERENCE_PROJECT\Source\srv\server\liblua. They are a complete supplied SDK source dependency, not claimed recovered original VM source. Original server ELF advertises Lua5.0/1994-2003; these supplied headers/archive version strings advertise5.0.3/1994-2006. Exact revision equality is not established.

The copied source/header/buildfile SHA and excluded prebuilt/user artifacts are indexed in reconstruction/quest_lua_query_evidence/package/sdk-provenance.json when the parent records the validated module. Original copyrights/license remain in SDKheaders. Prebuilt ELF32 archives are not linked into nativeMinGW tests; build all real28Cobjects as core20+auxiliary8 with the normal toolchain. Normal archives are generated liblua_mcncc.a/liblualib_mcncc.a.

Public API lua_Number=double8, original5.0 type tags/pseudoindices and queried symbols match role. NativeMinGW-m32 internalTObject16/value8 and lua_State96 differ originalFreeBSDTObject12/value4/state92 because of ABIalignment; keep SDK unchanged, do notpack/replacefields tofakeABI. Internal probe is explicit expectedmismatch evidence, not normal mandatoryABI. Full actualFreeBSDSDK/oldrevision equivalence remains unproved. No gamecallback registration, InitializeLua or ownership substitution is supplied here.

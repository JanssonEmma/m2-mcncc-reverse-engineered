# Locale predicate prerequisite metadata

Target-only eLocalization values in dwarf.json, corroborated locale_service.cpp/config.cpp: NOSET0,YMIR1,LETSGAME2,CIBN3,JAPAN4,ENGLISH5,HONGKONG6,NEWCIBN7,GERMANY8,KOREA9,FRANCE10,ITALY11,SPAIN12,UK13,TURKEY14,POLAND15,PORTUGAL16,CANADA17,BRAZIL18,MAX19. Enum size4.

g_eLocalType is EXTERNAL: DWARF variable DIE0x3130a2 DW_AT_external=true, type eLocalization; ELF STB_GLOBAL,size4,address083225b0,bsszero initialLC_NOSET. Do not make static. g_iUseLocale is ELF STB_GLOBAL,size4,address08322548,bsszero. Its target DWARF declaration was not found in selected locale_service/config/locale CUs; header proposal's int declaration is tentative pending actual declaration metadata.

Exact addressed predicates: LC_GetLocalType08068960,LC_IsLocale08068970,LC_IsYMIR08068990,LC_IsLetsGame080689b0,LC_IsCIBN080689d0,LC_IsJapan080689f0,LC_IsEnglish08068a10,LC_IsHongKong08068a30,LC_IsNewCIBN08068a50,LC_IsGermany08068a70,LC_IsKorea08068a90,LC_IsEurope08068ab0,LC_IsCanada08068b00,LC_IsBrazil08068b20. All return bool except GetLocalType enum. IsLocale takes enum. Europe membership must come from actual binary body, not newer code.

Partial header proposal locale_service.h includes only recovered enum,predicate declarations,externalglobal declarations (g_iUseLocale type caveat above). No F source edited or original binary executed. Root owns actual predicate body recovery.

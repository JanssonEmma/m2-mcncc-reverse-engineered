# CHARACTER staged recovery

Promote only owned files: game/src/char.h, char.cpp, vid.h, affect_flag.h and merge tables-char-fragment.h INSIDE existing common/tables.h include guard. Other files under this staging tree are dependency snapshots copied for local compilation; their owning workers/root manage promotion.

char.h includes all154 actual old CHARACTER members in original order with target DWARF access, all3 real bases, complete actual virtual interface, public defaultconstructor/destructor declarations, nonvirtual GetName/Disconnect declarations. No bytepadding placeholders or method-only class substitutes. Original nested TBattleInfo/SAttackLog/SAttackedLog declarations are present. Nonvirtual methods without recovered bodies are intentionally not implemented.

Embedded character_point1056/character_point_instant1688,TSkillUseInfo32,CTrigger8,VID8,TAffectFlag8 and packedSQuickslot2 are declared from actual DWARF members/bounds. Original char.h placement retained for point/trigger/skill structs; vid.h and affect_flag.h retain reference filenames. Pointer-only classes use legitimate forward declarations; original typedef spelling and standard container types retained. All virtuals remain declaration-only pending actual behavior; no fake constructors/destructors introduced.

char.cpp contains only real GetName at08127930 using root-confirmed body: if nameempty choose mob locale name or emptyliteral, otherwisec_str. Old nonconst member signature is preserved. CMob class is provided by another worker's full-data declaration, not inferred memory offsets.

Checks passed with MinGW GCC13 C++03, -m32,-D_GLIBCXX_USE_CXX11_ABI=0,-D_USE_32BIT_TIME_T,-Wall,-Wextra,-Werror: compilechar.cpp toobject; abi.cpp syntax compile verifies point1056,instant1688,VID8,flag8,trigger8,skilluse32,quickslot2,battleinfo8,attacklog8,attackedlog8,horse24. No original binary execution, no Fwrites.

Actual CHARACTER target3812 is NOT asserted with modern STL. Old map/set12 and list4 differ from currentSTL, as does baseCEntity'smap; source preserves semantic types while target ABI metadata remains separate. No full server link/runtime or CHARACTER construction claimed. GetName runtime branch tests require an honestly initialized CHARACTER; avoid treating rawmemory asconstructedC++object.

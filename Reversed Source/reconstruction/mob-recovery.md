# CMob lifecycle and splash recovery

Default constructors080f5010/080f5060 initialize the five vector members through normal C++ construction but leave the packed table and scalar skill fields untouched. This differs from the supplied newer source, which zeroes them. Default construction must not be used as a zero-initialization guarantee. Tests supply explicit scalar/table data and do not inspect indeterminate values.

The const CMob pointer constructor080f50b0/080f5240 copies exactly255 table bytes, then overwrites table Skills[2], [3] and [4] with vnum256 and level25. It separately copies all five runtime skill records and their vectors. These fixed stores are corroborated by assembly at offsets235/239,240/244 and245/249. This pointer-based constructor is separate from the language's implicit reference copy constructor. mob-constructor-types.json confirms the const pointer parameter from DWARF.

Destructor080f53d0/080f5440 has no application body beyond normal destruction of the vector array. The reconstructed empty destructor body therefore invokes real vector cleanup automatically; it is not a substitute for unknown logic. SMobSplashAttackInfo field construction is corroborated by the inlined temporary inside AddSkillSplash, with no separately emitted body claimed.

AddSkillSplash080f54b0 ignores indices outside0..4, logs accepted calls with the old SkillIndex format, and appends timing/distance without altering skill vnum/level. The standalone regression tests all indices, extreme invalid indices, repeated growth, packed table clone overrides, scalar/vector copying and independent storage after mutation. Character integration tests now link this real module rather than a test lifecycle replacement.

This recovers CMob behavior only. CMobManager/CMobInstance and mob table loading are not reconstructed. Host tests and32-bit layout checks do not establish full server equivalence.


Registry/instance update: complete CMobInstance24-byte data and CMobManager64-byte target data declarations are now present. CMobInstance constructors080f5560/080f5590 take two separate millisecond samples and clear three flags; the position remains untouched. Manager construction/destruction follows actual singleton/map lifetimes and does not delete pointed-to objects. Lookup080f6530 and GetGroup080f6b60 do not insert missing keys. Name lookup080f6580 is exact and case-sensitive; abbreviation returns the first sorted key matching the byte prefix, and an empty prefix matches the first entry. Tests cover high keys, prefix ambiguity, empty registries and non-owning destruction. Both Get overloads return const CMob pointers, confirmed by mob-get-signatures.json. Full manager layout differs with modern maps; instance24 and all field offsets are checked.

Initialize evidence is staged in mob-manager-registry-ghidra.json but is not implemented: it mutates input table skills, registers quest NPCs, loads group files and rebinds active characters. Loader/rebind/quest dependencies remain unresolved. CMobManager::Destroy has no resolved emitted function and is not replaced by a guessed body.


Asset-loading follow-up: the full old hierarchical parser/pool and motion/file/monster-skill loading are now reconstructed with real-file tests. See text-loader-recovery.md and motion-file-recovery.md. Motion table Build remains pending.


Table-build follow-up: the old Build and filename helpers are now reconstructed and tested with actual fixture assets. Exact old enums, shaman-path duplication and mob2493 skill rules are preserved. See motion-build-recovery.md. This supersedes earlier notes saying Build was pending.

# Packet-info DWARF findings

Original game ELF, packet_info.cpp CU only. Read-only extraction; no Ghidra calls or original binary execution. Complete extracted nodes and method parameters: dwarf.json; extractor: extract.py.

SPacketElement DIE 0x203d29d is 20 bytes, with iSize int at 0; stName std::string at 4 (old four-byte COW string); iCalled int at 8; dwLoad DWORD at 12; bSequencePacket bool at 16 (one byte, trailing three padding bytes). TPacketElement aliases it. The 2009 binary DOES contain the sequence flag.

CPacketInfo DIE 0x203eb1e is 24 bytes: vptr at 0; m_pPacketMap at 4, old std::map<int,TPacketElement*> size12; m_pCurrentPacket at16, TPacketElement* size4; m_dwStartTime DWORD at20. The three user fields have protected accessibility. No extra sequence member exists in the class; the per-element flag is present.

Methods:
- CPacketInfo() has constructor variants at0813bcf0/0813be00.
- virtual ~CPacketInfo() (see raw metadata for destructor variants).
- void Set(int header,int size,const char *name,bool sequence) @0813bff0; mangled _ZN11CPacketInfo3SetEiiPKcb.
- bool Get(int header,int *size,const char **name) @0813c0d0.
- bool IsSequencePacket(int header) @0813c150. Exact OLD name differs from newer IsSequence. No SetSequence declaration was found in this CU class.
- TPacketElement *GetElement(int header) @0813c1a0.
- void Start() @0813c1f0; void End() @0813c240.
- void Log(const char *filename) @0813c270.

Container ABI caveat: -m32 and _GLIBCXX_USE_CXX11_ABI=0 reproduce the four-byte old string but do not promise the GCC3.3 twelve-byte std::map representation. Modern libstdc++ uses a different tree/header representation. Preserve the source std::map semantics and separately document old layout; do not claim complete CPacketInfo binary ABI equality from scalar or string checks. SPacketElement can be structurally checked with the existing old-string 32-bit compile setup. Native host layout is also not the target layout.

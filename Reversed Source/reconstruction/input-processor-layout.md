# Input processor metadata

Original game ELF; input.cpp and desc.cpp focused DWARF extraction, no Ghidra calls, execution or F edits. Raw nodes in dwarf.json include all method parameter types and vtable slot expressions.

CInputProcessor (input.cpp DIE0x119d966) is12 bytes: vptr@0,protected CPacketInfo* m_pPacketInfo@4,protected int m_iBufferLeft@8. No embedded CG registry member exists. Ctor variants080cd000/080cd080 (input.cpp166); BindPacketInfo080cd100; Process080cd110; dtor080cf820/080cf830 (input.h28). Virtual slot2 bool Process(DESC*,const void*,int,int&), slot3 BYTE GetType(), protected slot4 int Analyze(DESC*,BYTE,const char*). Destructor slots precede these; virtuality attribute1 is GCC metadata and does not alone establish pure declaration. Other public declared helpers: Pong(DESC*),Handshake(DESC*,const char*),CRCReport(CHARACTER*,const char*),Version(CHARACTER*,const char*).

CInputClose (desc.cpp DIE0x1fc81db) size12, public CInputProcessor base@0, no addedfields. GetType slot3@0813b340; protected Analyze slot4@0813b350; destructor variants0813b310/0813b330. Default ctor implicit in saved metadata (no standaloneaddress).

DESC dependencies have declarations: CInputProcessor* GetInputProcessor(), socket_t GetSocket(),const char* GetHostName(),DWORD GetHandle(),LPCHARACTER GetCharacter(),bool IsPhase(int),BYTE GetSequence(),void SetNextSequence(). Scalar/socket getters are source-inlined caller evidence rather than standalonefunctionaddresses except GetSequence0813acc0 and SetNextSequence0813ace0. GetInputProcessor is inlined at080cd2cc, GetSocket at080cd3b3/080cd4d7/080cd7d0. Do not mistake those inline addresses for standalonegetter implementations.

ELF gc_abSequence global symbol08205860 has size65536 in rodata; sequence.bin contains exact extracted bytes and sequence-metadata.json hash/reference comparison. Newer reference has SEQUENCE_MAX_NUM32768 and must not be copied blindly. Variable type DIE was not found in selected CUs. Actual wrapbound and sequence indexing must be established from saved getter/next assembly/decompilation before reconstructing.

Still-real dependencies for Process: target global packet registry bound by ctor, GetSequence/SetNextSequence+exacttable, DESC SetPhase/other state calls, TrafficProfiler Report if its input path exists, packet registry Get/Start/End/IsSequencePacket, logging and the actual virtual Analyze/GetType dynamic implementations. Full concrete inputs beyondClose remain separate modules; no replacement production Analyze bodies should be added.

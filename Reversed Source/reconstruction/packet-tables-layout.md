# Derived packet registry DWARF evidence

Original game ELF, selected input.cpp/input_p2p.cpp/input_udp.cpp/packet_info.cpp CUs; no Ghidra calls, execution, or F edits. Raw metadata dwarf.json; complete packet structure sizes available in packet-sizes.csv (includes packet/table types visible in those CUs, not a claim every type is registered).

All three derived registries are 24 bytes and inherit CPacketInfo at offset0 without new members.

- CPacketInfoCG class DIE0x119d80f: input.cpp line51; constructor input.cpp line58 (080cb600 and080cc2a0); destructor line162 (080ccf40/080ccf80/080ccfc0).
- CPacketInfoGG class DIE0x1d88c73: input_p2p.cpp line24; constructor line31 (0811b230/0811b630); destructor line62 (0811ba30/0811ba70/0811bab0).
- CPacketInfoUDP class DIE0x21c6325: input_udp.cpp line37; constructor line44 (0813fd70/0813fdc0); destructor line49 (0813fe10/0813fe50/0813fe90).

Constructors/destructors each have DW_AT_inline=2: DW_INL_declared_not_inlined. Their declaration filenames resolve to the respective input source, not packet_info.h. Combined with class source lines and duplicated emitted constructor variants, this supports original inline class definitions located in input.cpp/input_p2p.cpp/input_udp.cpp. It does not mean the functions were optimized inline (the attribute specifically says declared, not inlined).

Reference source has CG/GG definitions in packet_info.cpp and UDP in input_udp.cpp. To preserve the requested reference source organization, placing reconstructed CG/GG declarations in packet_info.h and definitions in packet_info.cpp is a documented placement choice; the old binary's source provenance is the input files above. No behavior difference from relocation alone is implied.

ABI caveat inherited unchanged: modern libstdc++ tree representation differs; 24-byte target class size cannot be asserted for a class containing native modern std::map. All packet structure size entries are target DWARF values.


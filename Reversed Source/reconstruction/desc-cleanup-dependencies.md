# DESC cleanup dependency metadata

Read-only DWARF extraction from the original game ELF; no execution and no F source edits. Raw evidence: dwarf.json. Saved Ghidra body consulted: reconstruction/desc-lifecycle-ghidra.json entry 08139340.

## Exact recovered types

- CLoginKey is a 12-byte structure in both desc.cpp DIE 0x1f7f67c and desc_manager.cpp DIE 0x23ad8c4. Members: DWORD m_dwKey at 0, DWORD m_dwExpireTime at 4, LPDESC m_pkDesc at 8. DWORD is unsigned long, 4 bytes in this binary. Declared methods include CLoginKey(unsigned long, DESC *), void Expire(), and operator DWORD() const. No vtable or base class appears. Destroy contains the inlined Expire effects: set expiration to get_dword_time(), then clear m_pkDesc.
- SLogoutPacket is 48 bytes: char login[31] at offset 0, char passwd[17] at offset 31. TLogoutPacket is its typedef. desc.cpp DIE 0x1f65871/0x1f658fb, corroborated in desc_manager.cpp. No padding occurs between these character arrays.
- void DESC_MANAGER::DisconnectAccount(const std::string &) (_ZN12DESC_MANAGER17DisconnectAccountERKSs), implementation address 0x08158b70. Manager has old target size 128; only a method-only class substitute would misrepresent its actual layout.
- void CHARACTER::Disconnect() (_ZN9CHARACTER10DisconnectEv), implementation address 0x08129350. Old signature has no reason string parameter. CHARACTER is 3812 bytes in this DWARF.
- void CLIENT_DESC::DBPacket(BYTE, DWORD, const void *, DWORD) (_ZN11CLIENT_DESC8DBPacketEhmPKvm), implementation address 0x0813fbd0. CLIENT_DESC target size 588. Destroy passes header 2, descriptor handle, &logout_packet, 48.
- void DESC::Log(const char *, ...) (_ZN4DESC3LogEPKcz), implementation address 0x0813a5c0. Has variadic marker (not emitted by extraction child filter).

## Real Destroy prerequisites and order

1. Optional login key expiration/clear.
2. If account ID is nonzero, create temporary std::string from account login and invoke singleton<DESC_MANAGER>::instance().DisconnectAccount(login).
3. Close optional m_pLogFile, then clear it.
4. Invoke optional CHARACTER::Disconnect(), then clear m_lpCharacter.
5. Unconditionally buffer_delete(output), then buffer_delete(input); no pointer clearing and no buffered-output deletion.
6. event_cancel ping then delayed-disconnect.
7. If non-auth server and both first login/password bytes are nonzero, copy and terminate 30/16 byte strings and call DBPacket with the 48-byte logout record.
8. If socket != -1, sys_log, DESC::Log, fdwatch_del_fd, socket_close, then set socket=-1.

Do not add production no-op bodies for manager, character, database, or logging to force linking. Method declaration fragments for large classes are not complete reconstructed class definitions and should stay test-only if used as boundary doubles. Production Destroy should wait for sufficient real dependency declarations/layouts or an explicit partial-declaration strategy documented as incomplete.

Unknown here: globals g_bAuthServer/db_clientdesc were not found in the selected CU DWARF scan; their actual declaration/storage types require a broader metadata/symbol pass. Saved decompiler accesses g_bAuthServer as one byte and db_clientdesc as CLIENT_DESC pointer. None of this establishes full Destroy runtime validation.


ELF symbol-table follow-up: g_bAuthServer is a one-byte global object at 0x083231c1; db_clientdesc is a four-byte global object at 0x083258e4. This confirms storage widths and locations, but does not by itself resolve the former's bool versus char source declaration.

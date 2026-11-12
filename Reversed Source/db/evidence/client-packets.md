# Client packet dispatch

The method at `0x0805c400` was reconstructed from the isolated Ghidra export and checked against the ELF disassembly and complete 256-entry jump table at `0x080c43fc`. The unpacked ELF SHA-256 is `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. Staged source is `work/client-packets.cpp`.

- All 84 recognized numeric labels match the original table. The other 172 target default at `0x0805cec6` and log the unknown header, handle and length. Processing continues until no complete packet remains. The final `RecvEnd` consumes exactly the cursor from `PeekPacket`.
- Each complete packet increments the count and sets `m_bLastHeader` (original object offset 336). Test-server logging skips opcode 10. Per-command logs retain the original levels and text. Country IP opcode 127 logs before the switch and again before both responses.
- Opcode 114 accepts the old key and delay fields, each four bytes on the target. With a nonzero saved delay, the original delays once before the next read loop and increments below a lazily selected maximum. The separately guarded random initializers use `rand() % 5 + 1` and `rand() % 30000` in that order, verified at `0x0805cf90` and `0x0805cf55`.
- Opcodes 117?126 are the ten excluded Monarch operations. The staged source consumes those packets and skips their handlers. Other known opcodes retain original handler calls. Typed payloads rely on original packet layouts and the complete-frame check in `PeekPacket`, as in the binary.
- The merge requires the declared manager methods below, `m_bLastHeader`, `g_test_server`, `CPeer::PeekPacket`, `CPeerBase::RecvEnd`, the old shared packet types, `BlockCountry.h`, and the standard rand, usleep and memcpy interfaces. No separate routing method is called.

| Opcode | Branch address | Dispatch |
| --- | --- | --- |
| 2 | 0x0805c526 | QUERY_LOGOUT |
| 3 | 0x0805c545 | QUERY_PLAYER_LOAD |
| 4 | 0x0805c587 | QUERY_PLAYER_SAVE |
| 5 | 0x0805c5c9 | __QUERY_PLAYER_CREATE |
| 6 | 0x0805c620 | __QUERY_PLAYER_DELETE |
| 9 | 0x0805c50e | QUERY_BOOT |
| 10 | 0x0805c662 | QUERY_PLAYER_COUNT |
| 11 | 0x0805c67a | QUERY_QUEST_SAVE |
| 12 | 0x0805c6bc | QUERY_SAFEBOX_LOAD (safebox) |
| 13 | 0x0805c6e1 | QUERY_SAFEBOX_SAVE |
| 14 | 0x0805c71c | QUERY_SAFEBOX_CHANGE_SIZE |
| 15 | 0x0805c73b | QUERY_EMPIRE_SELECT |
| 16 | 0x0805c75a | QUERY_SAFEBOX_CHANGE_PASSWORD |
| 19 | 0x0805c779 | GuildSkillUpdate |
| 20 | 0x0805c791 | GuildExpUpdate |
| 21 | 0x0805c7a9 | GuildAddMember |
| 22 | 0x0805c7c1 | GuildRemoveMember |
| 23 | 0x0805c7d9 | GuildChangeGrade |
| 24 | 0x0805c7f1 | GuildChangeMemberData |
| 25 | 0x0805c809 | GuildDisband |
| 26 | 0x0805c821 | GuildWar |
| 27 | 0x0805c839 | GuildWarScore |
| 28 | 0x0805c851 | GuildCreate |
| 30 | 0x0805c86b | QUERY_ITEM_SAVE |
| 31 | 0x0805c883 | QUERY_ITEM_DESTROY |
| 32 | 0x0805c89b | QUERY_ADD_AFFECT |
| 33 | 0x0805c8c8 | QUERY_REMOVE_AFFECT |
| 34 | 0x0805c8f5 | QUERY_HIGHSCORE_REGISTER |
| 35 | 0x0805c90d | QUERY_ITEM_FLUSH |
| 36 | 0x0805c925 | QUERY_PARTY_CREATE |
| 37 | 0x0805c93d | QUERY_PARTY_DELETE |
| 38 | 0x0805c955 | QUERY_PARTY_ADD |
| 39 | 0x0805c96d | QUERY_PARTY_REMOVE |
| 40 | 0x0805c985 | QUERY_PARTY_STATE_CHANGE |
| 42 | 0x0805c99d | QUERY_FLUSH_CACHE |
| 43 | 0x0805c9b5 | QUERY_RELOAD_PROTO |
| 44 | 0x0805c9c2 | QUERY_CHANGE_NAME |
| 45 | 0x0805c9e1 | QUERY_SMS |
| 46 | 0x0805c9f9 | GuildChangeLadderPoint |
| 47 | 0x0805ca0d | GuildUseSkill |
| 48 | 0x0805ca21 | AddEmpirePriv |
| 49 | 0x0805ca35 | AddGuildPriv |
| 50 | 0x0805ca49 | MoneyLog |
| 51 | 0x0805ca5d | GuildDepositMoney |
| 52 | 0x0805ca71 | GuildWithdrawMoney |
| 53 | 0x0805ca89 | GuildWithdrawMoneyGiveReply |
| 54 | 0x0805ca9d | AddCharacterPriv |
| 55 | 0x0805cab1 | SetEventFlag |
| 56 | 0x0805cac5 | QUERY_PARTY_SET_MEMBER_LEVEL |
| 57 | 0x0805cadd | GuildWarBet |
| 60 | 0x0805caf1 | CreateObject |
| 61 | 0x0805cb05 | DeleteObject |
| 62 | 0x0805cb1b | UpdateLand |
| 70 | 0x0805cb2f | MarriageAdd |
| 71 | 0x0805cb43 | MarriageUpdate |
| 72 | 0x0805cb57 | MarriageRemove |
| 73 | 0x0805cb6b | WeddingRequest |
| 74 | 0x0805cb7f | WeddingReady |
| 75 | 0x0805cb93 | WeddingEnd |
| 100 | 0x0805cba7 | QUERY_AUTH_LOGIN |
| 101 | 0x0805cbc6 | QUERY_LOGIN_BY_KEY |
| 104 | 0x0805cbe5 | BillingExpire |
| 105 | 0x0805cbf9 | VCard |
| 106 | 0x0805cc0d | BillingCheck |
| 107 | 0x0805cc21 | QUERY_SAFEBOX_LOAD (mall) |
| 108 | 0x0805cc2f | MyshopPricelistUpdate |
| 109 | 0x0805cc43 | MyshopPricelistRequest |
| 110 | 0x0805cc64 | BlockChat |
| 114 | 0x0805cc78 | Legacy Hammer-of-Tor delay setting |
| 115 | 0x0805cc94 | ReloadAdmin |
| 116 | 0x0805ccac | BreakMarriage |
| 117 | 0x0805ccc4 | Election (excluded) |
| 118 | 0x0805cce3 | Candidacy (excluded) |
| 119 | 0x0805cd02 | AddMonarchMoney (excluded) |
| 120 | 0x0805cd21 | TakeMonarchMoney (excluded) |
| 121 | 0x0805cd40 | ComeToVote (excluded) |
| 122 | 0x0805cd5f | RMCandidacy (excluded) |
| 123 | 0x0805cd7e | SetMonarch (excluded) |
| 124 | 0x0805cd9d | RMMonarch (excluded) |
| 125 | 0x0805cdbc | DecMonarchMoney (excluded) |
| 126 | 0x0805cddb | ChangeMonarchLord (excluded) |
| 127 | 0x0805cdfa | SendBlockedCountryIp and SendBlockException |
| 128 | 0x0805ce9d | BlockException |
| 255 | 0x0805cefe | QUERY_SETUP |

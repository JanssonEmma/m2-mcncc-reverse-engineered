# CHARACTER chat recovery

ChatPacket at 0812fe70 now executes the formatter, real TEMP_BUFFER and descriptor output. The old chat packet is packed 9 bytes: header at0, size at1, type at3, ID at4, empire at8. Header is4, ID is0, empire comes from the descriptor, and text includes its NUL terminator. A missing descriptor returns before formatting. The test-server command diagnostic retains the original SNED spelling.

The original formatter uses an 86-byte local buffer with vsnprintf limit85, but uses the formatter return length without clamping for subsequent copying. That unsafe truncation behavior is retained; regression inputs cover only safe lengths through84, including empty text and observer command output. This recovery is not a security hardening change.

CHARACTER lifecycle, protocol virtuals and Disconnect remain explicit test-only boundaries. ChatPacket and state scheduling are production implementations, no longer fixture substitutes. Original CHARACTER layout depends on historical STL; modern host tests do not establish complete ABI or runtime equivalence.

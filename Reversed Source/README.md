# Metin2 server reconstruction

This tree reconstructs the supplied 32-bit FreeBSD game binary. It is an incomplete source reconstruction, not yet a buildable full server. Native Windows support is deferred.

The game/src, common, libthecore/src, libthecore/include, libgame/src and libgame/include paths follow the supplied newer reference tree. The reference tree is unchanged.

See reconstruction/CURRENT_STATUS.md for implemented modules, validation and remaining work. Binary evidence, address inventories and recovery notes live in reconstruction/. The reconstruction/decompiled directory contains analysis pseudocode, not compilable production source.

Run make check check-abi in reconstruction/tests using GNU C/C++. This builds isolated regression fixtures, not the server.

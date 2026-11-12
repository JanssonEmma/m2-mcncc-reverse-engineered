# Original spatial manager and lookup closure

The Ghidra target matched SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Complete assembly and call evidence is saved alongside the recovered source. The workspace owner header is based on the earlier raw DWARF extraction and checked again against the original method access and artificial declarations.

The original SECTREE_MANAGER is 52 bytes with a singleton base at 0, the map pointer registry at 4, area map at 16, region vector at 28 and NPC-position map at 40. Its four member types and order are preserved. SECTREE_MAP is 52 bytes with its vptr at 0, public map base at 4 and setting at 16. Its native STL layout differs from the original toolchain. SMapRegion is 76 bytes, SSetting 36, TAreaInfo 20 and npc_info 16 in the original. The original data offsets and full API are retained in original-layout-api.json. The three private ForAttrRegion helpers remain private. The artificial manager copy and both artificial assignment declarations are represented by implicit native C++ members.

| Complete method or original variant | Address | Instructions |
|---|---|---|
| Manager ctor variants | 080b3960, 080b3a80 | 66 each |
| Manager dtor variants | 080b3bb0, 080b3dc0, 080b3fd0 | 146, 146, 149 |
| Manager GetMap | 080b41f0 | 27 |
| Manager Get by package | 080b4240 | 23 |
| Manager Get by coordinates | 080b4280 | 20 |
| Manager GetMapBasePositionByMapIndex | 080b5a50 | 41 |
| Map Find by package | 080b3730 | 25 |
| Map Find by coordinates | 080b3780 | 20 |

Both ctor variants construct the existing singleton and the four complete containers. All dtor variants perform reverse native member destruction and singleton cleanup. The deleting variant adds operator delete. The complete manager destructor releases map nodes without deleting pointer values. The SECTREE_MAP destructor is different and owns its SECTREE pointers. That destructor calls SECTREE::~SECTREE at 080a7130, which calls SECTREE::Destroy at 080a71f0. Destroy has a reachable CHARACTER::Disconnect call at 080a7325. The spatial-map owning lifetime therefore remains pending.

GetMap searches the unsigned map key without private-map normalization. Both coordinate lookup paths use unsigned 32-bit division by 6400 and packed 16-bit coordinates. The GetMapBasePositionByMapIndex body searches the live vector from its beginning. Signed values greater than 10000 divide by 10000. A match writes z=0, then x=sx and y=sy. Missing leaves the output intact. SetEventFlag calls it at 081a68db.

The staged native regression uses genuine manager lifetime and checks singleton cleanup, default containers, missing maps, a null-valued map node and region results including the 10000/10001 boundary, negative IDs, first duplicate, and unchanged output on miss. The manager also destroys its populated native registry containers. There is no fabricated map instance. Successful nonnull map lookup and coordinate packing remain supported by complete assembly and source review, with runtime coverage pending genuine map lifetime.

Workspace validation of the final staged source passed strict -m32 -std=c++03 -Wall -Wextra -pedantic -Wno-long-long syntax, normal C++03 link/runtime, and -O2 C++03 link/runtime. The final chained command exited 0 (chunk fbd6a0). It used Strawberry g++ and the real project sectree, entity, typedef and singleton headers. No project changes or original execution.

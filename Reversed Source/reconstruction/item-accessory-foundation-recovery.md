# Accessory socket foundation

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Recovered CItem::GetSocketCount (08168110), IsAccessoryForSocket (08168810), and SetAccessorySocketMaxGrade (081688d0) from complete saved target decompilations. Original signatures are embedded in item-accessory-foundation-evidence.json.

GetSocketCount counts the uninterrupted nonzero prefix of three sockets. Negative sockets count as occupied; a zero ends the count even if later sockets are occupied. IsAccessoryForSocket directly requires the prototype and accepts only type2/subtypes3,5,6. No null-prototype guard is added. SetAccessorySocketMaxGrade clamps its signed argument to0..3 via the recovered MINMAX and calls real SetSocket(1,value), without testing accessory type or suppressing unchanged writes.

The existing socket integration test now checks all eight occupancy masks, all65536 byte type/subtype pairs, signed max-grade extrema and unchanged writes. Max-grade cases exercise actual packet output, delayed save, developer logging and SQL audit with the established external API fixture. They deliberately use a non-accessory prototype to detect an invented predicate guard.

accessory-constants.json records original ELF symbol locations/sizes and decoded values for the four accessory arrays, plus exact raw Korean message bytes for future locale/chat recovery. These are evidence only; their extraction alone does not establish complete accessory behavior.

Accessory downgrade, its stat mutations, expiry start/stop and remaining equipment behavior are not implemented by this step. Production substitutes have not been introduced. Original ELF execution and native DB/OS behavior are outside these tests.

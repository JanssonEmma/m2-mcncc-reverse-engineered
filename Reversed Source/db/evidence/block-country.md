# BlockCountry evidence

Target SHA-256 85E673ABA60DA66B68A6844C9E29DE5090769249718A9046F868CC5E128FF111.

- 0x08092a10 compares the unsigned address inclusively against the two range endpoints. 0x08093410 uses inet_aton and treats parse failure as blocked.
- Constructor variants 0x08092a40 and 0x08092ad0 initialize the singleton and both vectors. Destructor variants 0x08092b60, 0x08092c30, and 0x08092d00 delete the IP records. The vector of exception login pointers is destroyed without freeing individual strings.
- 0x08092de0 uses two SQL_ACCOUNT queries. The first empty result returns false. The second empty result returns true. In the binary both SQL strings are copied with sprintf. IP records are 60 bytes with two 32-bit endpoints, a 50-byte strncpy, and an explicit terminator.
- 0x08093470 sends header 171 and 8 bytes. 0x08093530 uses command 1. 0x08093580 permits commands 1 and 2, sends header 172 with 32 bytes, and copies at most 30 login bytes with strncpy plus explicit termination.
- 0x08093610 uses strcmp and strdup, and 0x080936a0 frees the first matching exception before erase.
- The existing reconstructed common header already has the matching TPacketBlockException. The IP packet and header values are local pending shared protocol integration. Build and ABI validation are deferred.

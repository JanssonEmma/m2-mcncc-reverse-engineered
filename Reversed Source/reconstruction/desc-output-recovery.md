# Descriptor output slice

This is a partial descriptor reconstruction, not a complete DESC implementation.

## Source and evidence

game/src/desc.cpp implements only DESC::ProcessOutput at0x08139bb0. game/src/desc.h contains all actual members in target order, the four virtual method declarations in order (destructor, GetType, Destroy, SetPhase), and the output method declaration. Unrecovered nonvirtual API declarations are omitted. Constructors, destruction, phase changes and input processing have no production substitute bodies.

common/tables.h contains real, typed, packed old SSimplePlayer/TSimplePlayer and SAccountTable/TAccountTable definitions required by the descriptor's by-value account member. Player size is63 and account size333; old passwd capacity is17 bytes including terminator. These are partial common declarations, not a wholesale copy of newer tables.

desc-layout.json and desc-output-types.json retain member/type/accessibility/virtual-index and counter metadata. descriptor-callers-ghidra.json retains five caller/method bodies; descriptor-output-assembly.json corroborates output and flush control flow. The target is game SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c, inspected read-only. The original executable was never run.

## Exact output contract

Empty output returns zero without querying the watcher. Otherwise fdwatch_get_buffer_size receives the SOCKET DESCRIPTOR, not event index. Nonpositive capacity returns zero. The method calls the recovered signed-int MIN function to choose capacity versus buffered size and returns zero without a write if the result is zero.

It calls the real socket_write with a pointer to the unread buffer bytes. Zero means successful completion of that requested length. Only success updates max_bytes_written via the recovered signed MAX function and adds the length to total/current counters, consumes that exact length, then registers WRITE=2, oneshot=1 with this descriptor as client data if bytes remain. A final drain submits no new write registration. Negative socket result propagates without consuming data, changing counters or rearming. A nonpositive capacity similarly does not rearm, even if an incoming one-shot event was just consumed.

All three counters are target DWARF signed four-byte int globals from main.cpp. The source uses unsigned32 addition then the intended GCC signed conversion to reproduce instruction-level wrapping without signed-addition overflow optimization. Signed MIN/MAX are calls to the already recovered C utilities, not newer unsigned macros.

## Layout verification

Target DESC is580 bytes with51 declared source members plus its compiler-provided vptr. The account/player types are packed real fields, not opaque byte arrays. Eight old std::strings occupy four bytes each. m_LastTryTime is a four-byte time_t. Ordinary current host settings do not match this object layout.

The bundled modern GNU toolchain can reproduce the structural model using:
- -m32
- -D_GLIBCXX_USE_CXX11_ABI=0
- -D_USE_32BIT_TIME_T

desc_abi.cpp passes all51 descriptor member offsets, sizes DESC580/string4/player63/account333, and all25 member offsets of the packed player/account types. -Wno-invalid-offsetof permits GCC's inspection of the polymorphic non-POD class solely for these structural checks. This is not a FreeBSD SDK, old libstdc++ runtime or calling-convention equivalence claim. Native64-bit host regressions use their honest native class layout.

The descriptor source passes optimized32-bit object compilation with those compatibility flags. -Wno-long-long suppresses only the expected GNU C++03 extension diagnostic from the previously recovered uint64 rdtsc declaration in utils.h; it does not change types or generated output-method behavior.

## Tests and isolation

desc_output_unit tests the actual output method with controlled dependency returns, including empty/zero/negative capacity, socket errors, partial/final drain, signed MAX and counter wrapping.

desc_output_integration links the actual reconstructed descriptor method plus actual buffer.c, socket.c, fdwatch.c and utils.c. Only operating-system calls and unrelated lifecycle methods are fixtures. It drives a real watcher poll/result lookup, consumes a one-shot event, exercises finite EAGAIN/partial-send retry sequences through real socket_write, verifies consumed bytes and counters, checks rearm state/client pointers, then covers nonpositive capacity, fatal EINTR and a final buffer reset. O0/O2 method builds pass both suites.

desc_fixture.cpp is test-only: it supplies ctor/dtor/GetType/Destroy/SetPhase bodies to construct a valid C++ object for isolated output tests. It does not pretend to model Initialize, cleanup, phase changes or protocol handling. These definitions never appear in production files. Actual unused OS boundaries assert rather than opening connections, polling real descriptors, resolving DNS, sleeping or forking.

## Remaining descriptor work

The source module is intentionally incomplete. Constructor/Initialize/GetType/destructor/Destroy and SetPhase need their real behavior recovered before production descriptor construction and linking. FlushOutput has retained evidence and is a bounded next method after phase handling. Setup/Input and the full io_loop need locale/handshake/event, DESC_MANAGER, packet metadata and concrete input processors; those remain real unresolved dependencies. CInputProcessor is a pointer in the old DESC layout, not an embedded object.

The original first layout assessment's modern-string concern is resolved for compile-only checks by the compatible GNU string/time flags above. It remains a real build configuration requirement. No claim is made that the full server now compiles or runs.

## Lifecycle follow-up

Constructor, destructor entry, Initialize and GetType are now recovered; see desc-lifecycle-recovery.md. Earlier lifecycle-fixture scope applies only to Destroy and SetPhase after this update.

# Temporary buffer manager recovery

Target game SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Read-only Ghidra/DWARF inspection; no original server execution.

game/src/buffer_manager.cpp/.h reconstruct the old BUFFER_MANAGER and TEMP_BUFFER classes. common/singleton.h supplies their checked singleton dependency at the reference's relative location. The newer supplied game source removed BUFFER_MANAGER and added TEMP_BUFFER size/force-delete/reset features; those newer APIs are not present in this target.

buffer-manager-ghidra.json retains 20 application, ABI-variant, singleton and startup decompilations. The compilation unit's other eight addressed entries are standard-library deque/stack algorithms; they are supplied by the C++ standard library rather than manually transcribing GCC internals. No claim of original libstdc++ binary compatibility is made.

buffer-manager-dwarf.json supplies class/type/declaration records and ELF global symbols. buffer-manager-static-init-ghidra.json confirms automatic startup and exit. BUFFER_MANAGER is size44 on the target: public singleton base at0 (vptr, size4), protected std::stack<LPBUFFER> at4 (size40). TEMP_BUFFER has only protected LPBUFFER buf at0, size4, a default constructor and nonvirtual destructor. BUFFER_MANAGER has a virtual destructor, public Destroy and protected get/release.

## Behavior

The local global buffer_manager at0x08323340 is automatically constructed before main and destroyed at exit. Its singleton pointer is the weak static object at0x082405c0. Tests use this automatic manager rather than constructing a second singleton.

get pushes buffer_new(NULL,8192) only when its stack is empty, then pops and returns the top. release resets a buffer and pushes it. Destroy pops each cached buffer and invokes buffer_delete in LIFO order. It is repeatable and does not invalidate buffers currently checked out. Destruction invokes Destroy, destroys the standard container, then clears the singleton pointer.

TEMP_BUFFER construction checks the manager singleton and gets a buffer; destruction releases it to the manager. read_peek, write and size forward to the C buffer module. No default-constructed null buffer, sized constructor, forceDelete field or reset method was added. Grown buffers retain their capacity while cached, including sizes exceeding the lower C pool's retention limit; Destroy eventually hands them back to buffer_delete.

## Header reconstruction choices

DWARF confirms getptr's inline declaration but has no emitted standalone body. Returning buf is reference-guided source inference from the sole member and newer getter, not an independently recovered executable body. TEMP_BUFFER friendship is inferred because verified calls access protected BUFFER_MANAGER methods; it has no layout effect and no explicit friend record was retained.

The singleton reference calculates base adjustment through long pointer casts. The reconstruction uses static_cast<T*>(this) to avoid narrowing pointers on the test host; it agrees for the verified public, nonvirtual singleton base at offset0. Only this instantiation's behavior/layout is checked, not every future singleton subclass. instance, Instance and instance_ptr declarations are all DWARF-confirmed; the emitted constructor/destructor and inline assertion paths corroborate their state semantics.

The observed manager vtable at0x0821ec40 has offset-to-top0, typeinfo pointer0, and destructor slots0x080fb2f0/0x080fb3d0; singleton's table at0x0821ec50 similarly contains0,0,0x080fbe00,0x080fbe50. Modern RTTI metadata, standard-library allocation details, assertion diagnostics and static-initialization mechanisms need original toolchain/build flags for stronger parity. Compiler-generated iostream initialization from the original include environment was not manually recreated.

## Validation

buffer_manager_regression links the actual C buffer implementation and actual C++ manager. It verifies startup singleton access, TEMP_BUFFER forwarding, LIFO reuse, resetting on release, preserved grown capacity, live buffers across repeated Destroy, retention of buffers beyond131072 until cache cleanup, and 300 simultaneous/reused buffers crossing deque-node boundaries. Original implicit shallow-copy hazards are not silently changed; callers must not copy TEMP_BUFFER or the singleton manager.

Both C and C++ source units pass strict -m32 object compilation. check-abi verifies BUFFER32, BUFFER_MANAGER44, singleton4 and TEMP_BUFFER4 on the available 32-bit GNU ABI. All 18 regression suites pass. No complete server build, original FreeBSD runtime or exact allocator/layout behavior across arbitrary standard-library versions is claimed.

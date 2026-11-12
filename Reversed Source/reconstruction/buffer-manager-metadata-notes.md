# Buffer manager metadata

Target game SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c; read-only static inspection only. `extract.py` reads DWARF of buffer_manager.cpp and ELF symbols. Full raw type/member/method records retained in metadata.json.

TEMP_BUFFER: target size4, protected LPBUFFER buf@0; default constructor only, nonvirtual destructor, const void* read_peek(), void write(const void*,int), int size(), LPBUFFER getptr(). None are const member functions (this points to unqualified TEMP_BUFFER). getptr exists as declaration/no emitted standalone body; implementing return buf is reference-guided and directly implied by sole member/name, not recovered executable body. No reset, forceDelete, sized constructor in old DWARF.

BUFFER_MANAGER: target size44, publicly derives singleton<BUFFER_MANAGER> at0 (size4,vptr@0), protected std::stack<LPBUFFER> buffer_stack@4 (target stack40). Default constructor, virtual destructor, public void Destroy(), protected LPBUFFER get(), protected void release(LPBUFFER). No other explicit application members. Compiler-generated copy constructor/operator= declarations exist. These are not evidence of deliberate safe singleton copying.

singleton<BUFFER_MANAGER>: size4,vptr@0,public static BUFFER_MANAGER*ms_singleton; default constructor,virtual destructor; static BUFFER_MANAGER&instance(), Instance(); static BUFFER_MANAGER*instance_ptr(). Reference common/singleton.h uses assert, sets this with base offset, clears static in dtor. DWARF records same structure/method declarations. Original singleton header path: REFERENCE_PROJECT\Source\srv\server\common\singleton.h.

Automatic global manager IS present. Local object buffer_manager at08323340,size44 in .bss. Weak singleton pointer object _ZN9singletonI14BUFFER_MANAGERE12ms_singletonE at082405c0,size4 in .data. Static initializer helper080fb840 explicitly constructs buffer_manager when initialize_p=1,priority65535 and destroys it when initialize_p=0. Retained static-initialization-ghidra.json confirms calls. Registration wrappers at080fb8b0/080fb8d0. Tests should use BUFFER_MANAGER::instance(), avoiding second concurrent instance.

Vtable ELF objects: _ZTV14BUFFER_MANAGER0821ec40 size16; _ZTV9singletonI14BUFFER_MANAGERE0821ec50 size16. Two destructor entries expected from Itanium ABI size; slot contents not extracted by this metadata pass. Modern std::stack/deque layout differs from original GCC3.3.3; source should preserve stack semantics, with old44-byte manager layout recorded as evidence rather than guaranteed under modern libstdc++.

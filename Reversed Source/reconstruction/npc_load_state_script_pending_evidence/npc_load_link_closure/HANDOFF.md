# Whole NPC loader utility link closure — read-only assessment

No source/project/helper/package edits or builds. Inspected actual complete libthecore/src/utils.c, tests/Makefile/static macro expansion, current desc_utils.o undefined symbols and desc_rng_os_boundary.o symbols using nm. No original ELF execution. This is link-plan evidence, not verified replacement/build.

## Smallest honest bounded option

Compile COMPLETE actual utils.c as existing desc_rng_full.o already does (-fno-lto -ffunction-sections -fdata-sections). Select complete unchanged MAX code+unwind+relocation sections into a TEST ONLY MAX object, with the SAME code/unwind SHA and relocation names/types/offsets equality proofs as project_rng.py. MAX's full source method is already reconstructed as int MAX(int a,int b){return a>b?a:b;}; never create a fakeMAX body or new application callback. On current x64 COFF choose .text$MAX/.pdata$MAX/.xdata$MAX; handle actual x86 symbol-prefix/section differences explicitly if supporting it, no retarget trick. MAX has no external application/OS calls; its object can be inspected/proved self-contained.

Put only that MAX object in a normal indexed archive, linked AFTER every ordinary source/object input. Add archive dependency and end-of-object archive link to all actualNPC.cpp recipes/sharedgraphs. Graphs already containing full desc_utils.o define MAX so archive member stays unextracted; older projectedRNG graphs load only complete genuine MAX. No duplicate thecore_random, clock functions or MINMAX. Link-order is essential: placing archive before later fullutils can force extraction then duplicateMAX. --whole-archive is forbidden here. Normal ar packaging of selected genuine COFF is suitable; do not merely archive FULL utils.o (one member includes allsymbols, extractingMAX would still collide with current testclock symbols/RNG).

Alternatively add exactMAX projected object ONLY to missingutilitygraphrecipes (nineprojectedRNG targets + isolatedconstructor after its broadergraphrepair). This avoids archive machinery but requires comprehensive classification and future sharedgraph maintenance. Never add MAX explicitobject to current fullutils graphs, which would duplicateMAX. First archive approach centralizes honest symbol resolution and preserves current boundary domains.

## Why linking full utils everywhere is larger scope

Existing isolatedfixtures intentionally define APP CLOCK BOUNDARIES: entity_packet_fixture.cpp53 get_dword_time,56 get_float_time; character_unused_clock.cpp4 aborting get_float_time and6 aborting MINMAX. Replacing desc_rng_only.o with desc_utils.o would duplicate THESE symbols, even after projectedRNG removal. Fullutils contains thecore_random too; retaining projectedRNG alongside fullutils duplicates it. A fullutils-only solution must first retire/rework these clock/MINMAX fixture methods and update test observer semantics against actual gettimeofday/nativeOS boundaries; this is a broader explicit behavior/domain change, not an incidental MAX link fix. Fullutils archive does not avoid whole-object duplicates.

Current desc_utils.o external symbols include fork,random,gettimeofday,select,_sys_err,thecore_memcpy plus C-runtime. Existing desc_rng_os_boundary.o from utils_link_stubs.c defines ONLY fork/random and asserts if reached. Those are explicit nativeOS boundaries, not MAX application replacements. Descriptor/fullgraph nativeOS observers already supply corresponding OS/log/memcpy edges. Do not add RNG or MAX fake stubs to new NPC tests.

## Exact affected recipe families (static macro audit)

Current Makefile has thirteen literal manager/NPC occurrence contexts inherited from101: one DESC_PACKET_SOURCES assignment, six standalone prerequisites and six standalone recipes. Additive arg constructor direct recipe is separate. Static expansion of uppercase source/object macros identifies69 C++ recipe graphs carrying actualNPC.cpp (not69 unrelated recoveryfeatures). Of those,59 already reference full desc_utils.o,9 projectedRNG,1 isolatedconstructor no utility object. This is static source-list analysis only, not an actual make execution or full parser/verification claim.

Nine projectedRNG recipes:
- desc_output_unit, input_regression, desc_flush_regression use character_unused_clock boundary (3).
- character_name_regression,entity_regression,character_initialize_regression,character_manager_regression,character_creation_regression,item_regression use entity_packet_fixture clock boundary (6).

All old fullutils graphs must also link complete new NPC method and real calleegraph; do not rely on unused-code/LTO deletion as closure. Constructor-only graph arg_script_constructor_regression currently links test+actualNPC.cpp only. Whole new LoadStateScript introduces real CQM queries/otherappcallees beyondMAX, so this graph MUST be expanded to actualfullappgraph (or source ownership legitimately reorganized into complete originaltranslationunits, which is larger structure change). MAX alone does not fix its closure. Existing constructor helper's direct test+NPC O2 command likewise must be explicitly rebased to realexpandedgraph or split actual method objects with whole-source completeness proven, never testconditional no-loader NPC.

## Required verification / batching

Treat loader+MAXlink policy+allsharedrecipe propagation+isolatedconstructorgraph/helper update as ONE globally shared integration batch after root's active build ends. Focus newfull loader test plus isolatedconstructor, one fullutils quest/NPC suite, three abortclock descriptor/input suites, ALLsix standalone entityclock contexts, and projection integrity/smoke for originalRNG/newMAX. Verify actual resolved MAX uniqueness and no duplicateRNG/clock methods in each family, with code/unwind/reloc equality receipts for selectedMAX object and no runtime OS-callback expansion claims. Strict32 compile actualnewNPC/headers; optimized helper must compile realMAX selected from fullyoptimizedutils and fullactualNPC/appgraph, not link normalMAX whileclaimingO2allrelevantobjects.

Then canonical FULL existing116+newloader suite(s)+ABI is required because global NPC.cpp and shared Makefile graphs changed. Do not run several consecutive full rebuilds for partial utility/constructor/loader stages; focused validation then one fullcombined run after all reviewed changes. Originalclock fixture domains remain explicitly isolated if selectedMAX option used. Fullutils-everywhere alternative needs its own broader fixture/domain review before samebatch, no silentfake compatibilityrepair.

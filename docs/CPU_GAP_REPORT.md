 Nebula Speedup Gap 
 RMGE01 native recompilation Analysis of Native22 2026-10-11 
 Where the 3-6× is still missing
 On the 9950X3D the simulation thread is 2.6 to 3.0 times cheaper than at Native11. The laptops have not been measured since Native10. On them, three things now set the limit: the size and shape of the generated code, runtime work still on the simulation thread, and a render thread that has barely changed.
 Measured from a run, profile or file 
 Derived read from code, disassembly or computed 
 Hypothesis not yet measured 
 Contents
 Short answer 
 Where we stand 
 Simulation thread now 
 Why game code is slow 
 Runtime work left 
 Render thread and GPU 
 Laptops and the i7-6500U 
 What closes the gap 
 Reading your recordings 
 Method 
 Short answer
 The desktop gain is real but sits at the low end Simulation thread 37.9 % of a core (Native11) to 12.7-14.5 % (Native22) on the gameplay route, at 60 frames per second Measured . The render thread has varied between 8 and 16 % across runs since Native12. Its structure has not changed.
 No hot spot is left in the game code No translated function is above 1.2 % of samples. What remains is spread across every function: the cost of each translated instruction Measured .
 The generated code is about 40 times the size of the PowerPC it came from The functions behind 80 % of game-code time are 7.7 MB of x86. The 185H's L3 is 24 MB, shared with everything else, and the i7-6500U's is 4 MB. The same module needs 1.5 times the CPU time on a 32 MB L3 as on a 96 MB one Measured .
 Two code patterns dominate First, every call moves the live guest registers through memory: about 22 memory operations at a virtual call site, and calls are one in twelve hot instructions. Second, float values go through memory with bit-manipulation conversions: about 18 to 22 x86 instructions per lfs or stfs Derived .
 A quarter of the simulation thread is still runtime work This is clock reads, checkpoint bodies, the PE pre-parse of the FIFO, the gather-pipe drain and wake syscalls Measured . The plan items that delete these outright have not started: the checkpoint-free generation, GX stage A, and storage and input as host calls.
 The i7-6500U is still short Scaling the Native22 desktop cost gives 9 to 17 ms of simulation work per frame against a 16.7 ms budget. It needs about 2 times more from the simulation thread and 1.5 to 2 times from the render thread to hold 60 with margin Hypothesis .
 Where we stand
 Measured on the 9950X3D unless noted. Percent of one core at 60 frames per second; one frame is 16.7 ms.
 Measure Then Native22 Evidence 
 Simulation thread, gameplay route 37.9 % (Native11) 12.7-14.5 %, 2.1-2.4 ms/frame Measured 
 Render thread, gameplay route 8-16 % since Native12 11.0-13.5 %, 1.8-2.3 ms/frame Measured 
 Simulation thread, Core Ultra 9 185H 61-74 % (Native10, Detailed) not measured Measured 
 Game module 382 MB (Native21) 209 MB Measured 
 Hot code, 80 % of game-code samples 7.7 MB in 592 functions Measured 
 Projected cost per frame
 Desktop milliseconds times the single-thread speed ratio (PassMark single thread: 9950X3D 4,739, Core Ultra 9 185H about 3,711, i7-6500U 1,686), times a cache factor. The factor is 1.5 as measured for 32 MB against 96 MB, assumed 1.5 to 1.7 for 24 MB and 1.5 to 2.5 for 4 MB. The render range leaves out the GPU driver, which costs more CPU on Intel graphics.
 CPU Simulation, ms/frame Render, ms/frame Evidence 
 9950X3D, 96 MB CCD 2.1-2.4 1.8-2.3 Measured 
 Core Ultra 9 185H 4.0-5.2 3-4 Hypothesis 
 i7-6500U at 3.0 GHz 8.9-16.9 6-10 Hypothesis 
 The 185H line agrees with the old data: Native10 on the 185H used 1.6 times the desktop share of the time (61 % against 37.9 %). Your recording will test it. On the i7-6500U the two threads together need 15 to 27 ms of CPU per frame on two cores that also run the driver, audio and workers. A safe target there is about 8 ms each, which means about 1.0 to 1.3 ms of simulation work per frame on the desktop: roughly half of today Hypothesis .
 The simulation thread now
 Exact profile, gameplay route, 124 s, 250 µs sampling, 35,236 running samples (generation 21 module, Native21 runtime).
 Running samples by owner. Windows rows overstate system calls, because a suspended thread is sampled where its system call returns. 
 Game code (translated and lean bodies) 62.5 % 
 Runtime 24.8 % 
 Windows and drivers 12.6 % 
 Largest runtime items, percent of running samples. Measured 
 Clock reads 3.3 % 
 Checkpoint bodies 2.6 % 
 PE pre-parse of the FIFO 2.5 % 
 Idle wait 2.2 % 
 Calls through the runtime 1.8 % 
 Guest write drain 1.0 % 
 Diagnostic records, gates 0.9 % 
 Gather pipe 0.55 % 
 Event latch lock 0.53 % 
 Rates in the Native22 package run: 56,000 clock reads per second, 9,600 of them by the game; 9,400 checkpoint bodies, 450 idle waits and 420 broker wakes per second Measured . These runs had the Lightweight-recording diagnostics on, which normal play turns off.
 Why the game code is still expensive
 The game code is two thirds of the simulation thread, and its profile is flat. The cost sits in how each instruction is translated, not in particular functions.
 Code size and cache
 x86 code behind the hottest functions, MB. Generation 21 module, functions sorted by samples, sizes from the linker map. 2,456 functions received samples. Measured 
 50 % of samples, 144 fns 2.3 MB 
 80 %, 592 fns 7.7 MB 
 90 %, 1,064 fns 11.8 MB 
 95 %, 1,509 fns 15.2 MB 
 100 %, 2,456 fns 22.1 MB 
 Function PowerPC x86, gen 23 Ratio 
 Vec::operator= , 3 loads and 3 stores 28 B 1,792 B 64× 
 OSDisableInterrupts 20 B 256 B 13× 
 KCollisionServer::checkArrow 1,800 B 95,104 B 53× 
 ShapePacketUserData::loadTexMtx 324 B 25,024 B 77× 
 The 361 hottest translated functions 141 KB 5.72 MB 41× 
 Hot code that does not fit the cache stalls the front end. That is the 1.5 times measured between the two CCDs, and the reason the laptops should gain more from size cuts than the 96 MB CCD shows Hypothesis .
 What one guest instruction costs
 Counted on the taken path in the disassembly of the shipped module, in Vec::operator= ( 0x80188E04 ) and NameObjCategoryList::execute ( 0x802617B4 ); float arithmetic from the helper source Derived .
 Guest instruction x86 now Direct compile Where the extra comes from 
 lfs ~20 2 Widening done in integer bit operations with a branch for special values, a store to the context, an HID2 test before the second-lane store, and a flat-base test 
 stfs ~22 2-3 Integer narrowing, the gather-pipe address test, the flat-base test 
 fmuls , fadds ~13 1 Two context loads, multiplier rounding, a double operation, two conversions, a context store, the HID2 test 
 stw to the stack 8-10 1 Gather-pipe test, flat-base test, a spill reload 
 lwz 1-2 1 Cheap on the taken path; the cost is the duplicate block described below 
 bctrl (virtual call) ~68 + callee 2-5 11 context stores, dispatch-table checks, 11 reloads with stack spills, a checkpoint test that re-reads the services table 
 Function entry and exit ~40 0-6 8 host register pushes and pops, about 12 context loads, base and interior-entry checks, stores of written registers 
 The compiler cannot prove the flat-view base is non-null, so every block that touches memory exists twice: once for the flat view and once through the region table, used only when there is no flat view. A fastmem build only ever runs the first copy. The second roughly doubles memory-heavy code and sits between the hot lines Derived .
 What the hot code is made of
 PowerPC instruction mix of the hot functions, percent. Decoded from main.dol , weighted by each function's samples (19,239). A static mix, not a dynamic count. Derived 
 Integer ALU 28.3 
 Integer loads 15.5 
 Branches 8.9 
 Integer stores 8.9 
 Calls 8.0 
 Float loads 6.3 
 System, CR logic 4.8 
 Integer compares 4.4 
 Returns 4.2 
 Float arithmetic 3.6 
 Float stores 3.2 
 Paired single, other float 3.7 
 60 % of the hot stores and 23 % of the hot loads address the stack through r1 or r11 , which can never reach the gather pipe or a device Derived . Weighting the costs above by this mix gives about 8 x86 instructions per guest instruction. About half of that is at calls and returns and about a quarter is float handling. A direct compile of the same code would be about 1.5 Hypothesis . That difference is where the remaining 3-6× lives.
 Calls are the largest single item
 Across the 150 hottest translated functions the generated source holds 1.3 context stores and 1.3 context loads per guest instruction Derived . The residency pass treats every call as a full synchronization point: it stores every register written so far, calls, then reloads every register used later. This is one virtual call in NameObjCategoryList::execute :
 context->gpr[0] = gr0; context->gpr[1] = gr1; context->gpr[3] = gr3; context->gpr[4] = gr4;
context->gpr[11] = gr11; context->gpr[12] = gr12; context->gpr[29] = gr29; context->gpr[30] = gr30;
context->gpr[31] = gr31; context->lr = glr; context->ctr = gctr;
galaxy::fast::call_guest_indirect(services, branch_target, ...);
gr0 = context->gpr[0]; gr1 = context->gpr[1]; ... gctr = context->ctr;
if (galaxy::call_return_checkpoint_due(services) != 0u) [[unlikely]] { ... }
 The PowerPC ABI that CodeWarrior follows makes most of this unnecessary. The callee preserves r14 to r31 ; r0 , r11 , r12 , CTR and XER are dead after a call; only r3 and r4 (and f1 ) come back. XenonRecomp, the recompiler behind Unleashed Recompiled, keeps non-volatile and non-argument registers as locals on that basis. Its README reports an executable about 20 MB smaller and "frame times are reduced by several milliseconds" ( XenonRecomp ). Nebula's earlier reason not to do this was flat transfers needing the full register file. The cooperative scheduler leaves 4 of them per run, all in OSInit Measured .
 Runtime work the plan removes, not yet removed
 From docs/PERFORMANCE_README.md , the program's work order. Items 1 to 3 and 7 are done, and 4 and 6 are partly done. These have not started.
 Plan item Deletes Cost today 
 5. Generation 18 without checkpoints Loop and call-return checkpoint tests, their shared cold blocks and interior entries, about 1.0 M checkpoint bodies per run Measured bodies 2.6 %; inline tests 1-2 % in an earlier A/B; plus code size 
 10. GX stage A PE register emulation and the simulation thread's FIFO pre-parse Measured about 2.5 % 
 8. DVD, IOS, NAND, SC as host calls DI and IPC register emulation Measured about 65,000 handled faults per run; device reads and writes about 0.5 % 
 9. WPAD and KPAD as host calls The Bluetooth stack and Wii Remote processing as translated code Measured WPadAcceleration alone 1.8 % of game-code samples 
 14. Leftovers Clock reads, diagnostic records and gates compiled into release (F18) Measured 3.3 % and about 0.9 % 
 The plan expects the simulation thread to drop noticeably once generation 18 lets resident registers survive calls. The census above shows the main cost at calls is the register-file synchronization itself. Removing checkpoints therefore pays off fully only together with the ABI-trusting call handling below Derived .
 Render thread and GPU
 The render thread still runs the full GX pipeline: FIFO parse, display-list caching with validation, per-draw state translation, CPU vertex decode, then the driver.
 Render thread, running samples by activity, percent. Same profile, 29,692 running samples, RTX 5090. Measured 
 GX to Direct3D state per draw ~16 % 
 FIFO parse and display-list replay ~13.5 % 
 Display-list and packet validation (memcmp, lookups) ~13 % 
 NVIDIA driver ~11.5 % 
 Waiting, condition-variable spin ~10 % 
 CPU vertex decode ~8 % 
 Heap allocation and free on the render thread: 3.4 % of its running samples Measured .
 In the package run, 3,171 mid-frame streaming attempts found the render queue full, against 15,551 that went through (6,152 against 16,778 in the profiled run). The render thread trails the game inside the frame Measured . On a slower CPU that becomes a wait at the frame boundary Hypothesis .
 Not started: GX stage B (host GX API, the WiiCompiled and Aurora model), the compact per-layout vertex format (F12; the decoder writes 144 bytes per vertex), and EFB copy scaling (P7).
 GPU at higher internal resolution on the 185H: at 3× there were 2,694 identity copies carrying 28.3 GB in 255 s (Native10). Every non-XFB copy is 9 times the pixels at 3× Measured . Internal resolution does load the GPU directly, but these copies spend that load on intermediates.
 Laptops and the i7-6500U
 Cache. 7.7 MB of hot code against a 4 MB L3 shared with data, the render thread and the driver.
 Two cores. The simulation thread, the render thread, the graphics driver's threads, audio and the decode and compile workers share two physical cores. Hyper-threading siblings slow each other when both are busy Hypothesis .
 Shared 15 W. CPU and HD 520 share one package power limit, so GPU work lowers CPU clocks Hypothesis .
 Not measured since Native10. The 185H then showed 20 to 105 ms idle-loop gaps whose cause was never pinned down. PE at consumption (Native11) and the cooperative scheduler (Native21) have changed that path since. Your recording will show whether the gaps are gone.
 What closes the gap
 Ranked by expected effect on small-cache CPUs. Every expected effect is a hypothesis until measured.
 Lean memory paths
 1 · one translator generation · low risk 
 Change Fastmem modules assume the flat view, so the second memory path and its tests leave every load and store. r1 / r11 -relative accesses skip the gather-pipe test. lfs , stfs and psq use hardware conversions, and the second lane is always written (HID2.PSE never changes in RMGE01). Checkpoint counters become module globals, so the test stops re-reading the services table after each call. 
 Evidence The instruction table above; 60 % of hot stores are stack stores. 
 Expect A large cut in module size and in instructions per memory operation Hypothesis . 
 Check Differential harnesses, frame dumps, then a steady-scene A/B on the 32 MB CCD. 
 Hot code laid out together
 2 · link order · no behaviour risk 
 Change Link functions in profile order (lld /ORDER from the exact profile, or emit shards hottest first) so the 7.7 MB hot set is contiguous instead of spread over 194 MB. 
 Expect Fewer instruction-cache and TLB misses on small caches Hypothesis . 
 Check Same A/B; also the 4-logical-CPU affinity run. 
 Registers that trust the ABI
 3 · largest game-code gain · medium risk 
 Change Non-volatile r14 - r31 stay locals: no save and restore through the guest stack and no reload after calls. Volatile non-argument registers are never written back. A call stores only the argument registers it sets and reloads only r3 , r4 , f1 and f2 . 
 Needs An opt-out list for functions that break the ABI, and the 4 flat transfers kept at full synchronization. A static liveness census of all 41,975 functions in main.dol found the candidates Derived . 4 read a volatile register after a call; 3 of them are debugger stubs that retail never runs. 14 read scratch registers at entry: exception vectors, OSSaveContext , the pointer-to-member thunk. 76 may read a caller's non-volatile register (0.6 % of samples). Some of those are false positives of a path-insensitive analysis, such as __GXSetSUTexRegs . 
 Expect Most of the call share of the generated instructions gone, and smaller functions Hypothesis . 
 Check Gameplay, Home menu, boot and save routes; frame dumps; flag-return counts. 
 Checkpoint-free generation
 4 · plan item 5 
 Change Deliver events at the cooperative scheduler's points only. Keep loop checkpoints only in loops that wait on memory without a call, found statically. 
 Expect Checkpoint bodies and tests gone, plus the shared cold blocks and interior entries Hypothesis . 
 GX stage A
 5 · plan item 10 
 Change GXDrawDone , GXSetDrawSync and GXSetDrawDone as host calls. The render thread's own parse finds the tokens. 
 Expect The 2.5 % pre-parse and the PE emulation off the simulation thread Hypothesis . 
 Render-thread quick wins
 6 
 Change Validate cached display lists by flush tracking instead of a byte compare. The game must DCFlushRange a display list before GXCallDisplayList ; writes through the uncached mirror need a check first. Pool the per-frame allocations. Rebuild constants and descriptor tables only when their inputs change. 
 Expect About a fifth of the render thread Hypothesis . 
 Later
 7-10 
 List Float registers as typed locals; inlining small leaf functions across shards (translator or ThinLTO); EFB copy scale cap and identity aliasing when a laptop recording shows GPU-bound frames; WPAD/KPAD and storage as host calls; on two-core CPUs, the simulation and render threads on different physical cores. 
 If items 1 to 4 make the game code 1.5 to 2 times faster and halve the runtime share, the desktop simulation thread lands near 6 to 8 % of a core. That projects to about 4.5 to 9 ms per frame on the i7-6500U Hypothesis .
 What I will read first in your recordings
 threads.csv : simulation and render thread share per window, the peak one-second windows, and the driver threads on the Arc iGPU.
 The simulation profile's module rows, resolved against generation 23's map, split into the same categories as above for laptop against desktop.
 [present-stats] : refresh-hz , refreshes per displayed frame, worst frame time.
 [idle-loop-sleep] and [checkpoint-gap-record] : whether the 20-105 ms gaps remain.
 timeline.bin : draw-done to PE finish and the GPU span per frame at each internal resolution; [efb-copy-work] ; [gx-stream-audit] .
 Decision rule: if the laptop simulation thread at 480p is above about 30 % of a core, items 1 to 4 come first. If the render thread is the larger one, item 6 comes first. If frames drop only at higher internal resolution with the GPU near full, the EFB copy work comes first.
 Method
 Profile: gameplay route, 124 s, 250 µs sampling, generation 21 module with the Native21 runtime, generated-code rows resolved with the module's linker map. Generation 23's changes are in the package smoke numbers, not in this profile.
 Static analysis: main.dol decoded for the instruction mix and stack share; generated C++ source of generation 23 for register traffic; llvm-objdump of the shipped RMGE01_game.dll for instruction counts. Scripts are in the session scratchpad ( gap/ ).
 Single-thread ratings: PassMark 9950X3D , PassMark i7-6500U , PassMark Core Ultra 9 185H . The i7-6500U has 4 MB of L3 and runs both cores at up to 3.0 GHz.
 Nothing was built or run for this report.
 Companion to the Native22 fixes report. Figures from the Nebula tree at commit fe9a7f3 and the runs in D:\NebulaWork\native\smoke .

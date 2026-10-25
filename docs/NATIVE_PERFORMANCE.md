# Native port: performance

The target is the slowest machine that can run Nebula. Every decision that costs time is listed here, with its cost
and the evidence.
- No speed-affecting design is decided or announced before the code paths it touches have been read and the cost
  measured, or labelled H with a measurement plan.
- **M**: measured here. **D**: read from source, assembly or generated code. **H**: hypothesis, with the measurement
  that would settle it.

Measurements:
- Machine: Ryzen 9 9950X3D, 4.3 GHz, Windows 11, clang 22, `-O3` with the module's flags, pinned to one core.
- Older and low-power CPUs forward stores and divide more slowly, so the ratios below are lower bounds there (H).
- Reproduce with `native/bench/hotpath_bench.c` and the `perf:` lines of `native/sdk/os/tests/run_tests.sh`.

## The start

Launch to the title screen (`boot:` lines, a start with no shader cache; stand-in: the 2-CU iGPU and two
cores with SMT of the machine above, `GALAXY_GPU_PREFERENCE=low GALAXY_TEST_CPU_AFFINITY=F`):

| | before | after | evidence |
|---|---|---|---|
| Stand-in | 2.86 s: Streamline 1.08 s loaded for a GPU that cannot run DLSS, the game's black frames before the title 1.08 s | 0.80 s | M |
| RTX 5090 | 3.37 s | 2.35 s (Streamline 0.78 s, beside the device's creation now, and 0.92 s handing it the device) | M |

- Streamline loads only for an NVIDIA RTX GPU, beside the device's creation.
- Before its first 3D scene the game shows nothing and its loads advance one step a retrace while each frame takes
  well under a millisecond (a profile of those frames: 94 % idle, waiting for the next retrace): until then retraces
  come every 2 ms and one frame a display period is shown. 65 frames take 0.29 s instead of 1.08 s; the title and its
  music start alike (the music 0.46 s after the title scene against 0.50 s).

## Fixed

| path | before | after | evidence |
|---|---|---|---|
| Re-enabling interrupts (`OSRestoreInterrupts`, `OSEnableInterrupts`; every heap allocation and mutex takes this path) | read the host clock on each call to check the decrementer: 20.6 ns | one word of pending work, one load and test: 1.46 ns per disable/restore pair, 6.9 ns per `OSLockMutex`/`OSUnlockMutex` | M |
| Decrementer (OSAlarm) | deadline compared on every re-enable | a host clock thread sets the decrementer's bit at the deadline; the game's own `OSGetTime`/`OSGetTick` reads also catch it | M |
| Time base (`OSGetTime`) | two 64-bit divisions per read | a 64.64 fixed-point multiply: 16.0 ns including the host counter | M |
| Wake-ups | `PPCMtdec` and `__OSUnmaskInterrupts` signalled the simulation thread's event (a system call) from the simulation thread itself | host threads signal only while the simulation thread sleeps | D |
| Timer resolution | `timeBeginPeriod(1)` at boot: raises the whole system's timer rate (power on laptops) | not used; high-resolution waitable timers (Windows 10 1803 and later) | D |
| GX command writes | a reservation per value: the write position lives in memory the byte stores may alias, so it is reloaded and stored per value. Matrix load 6.6 ns, vertex 3.2 ns | a reservation per command or call: register loads, matrix and light loads, `GXBegin`, `GXCallDisplayList`, the vertex functions, J3D's FIFO helpers. Matrix load 2.2 ns, vertex 1.4 ns | M |
| GD and J3D display-list writes | a load and store of the write position per byte | per value or command | D |
| Command stream to the renderer | copied out of the FIFO (9.3 ns/KB here, cache-resident) | spans of a double-mapped ring, parsed in place and then released | M (copy cost), D |
| PE tokens | v1.0.1 pre-parsed the stream on the game thread to find tokens (2.0 points of the simulation thread) | `GXSetDrawSync`/`GXSetDrawDone` report each token's position | D |
| `GXFlush` | 32 NOP bytes per flush (the write-gather buffer's size) | none | D |
| `__frsqrte` + the game's Newton step (`TUtil::sqrt`, vector normalisation) | `1/sqrtf`: 33 cycles dependent | `rsqrtss`: 17.5 cycles; worst relative error 2.1e-7 against 1.4e-7 (both about 3 ulp) | M |
| Square roots through the estimate alone | n/a | `sqrtss`, 13 cycles, exact | M |
| `NO_INLINE` (Petari's matching hint) | 499 functions forced out of line, `TUtil::sqrt` and `clamp` among them | the compiler decides | D |
| `sqrtf` and other math calls | math-errno on: compare, branch and library call on the error path | `-fno-math-errno`; the game never reads `errno` | D (assembly) |
| Disabling interrupts (`OSDisableInterrupts`) | n/a | delivers work pending since the last enable first, as the console would have taken it: the same one load and test, 1.47-1.60 ns per disable/restore pair (was 1.46-1.54) | M |
| Function-local statics (22 functions) | thread-safe first-use check: the TLS index, the thread's TLS block and the init epoch, three dependent loads and a compare per call | `-fno-threadsafe-statics`: one guard-byte test. All game C++ runs on the simulation thread; the host-thread entry points are C without such statics. No TLS read remains and the `.tls` section is gone | M (disassembly) |

The decrementer is late by the host timer's granularity when nothing reads the clock:
- 1 ms alarm, thread busy: 438-483 µs late on average, 587 µs at worst.
- Thread busy and reading `OSGetTime`: 0.1 µs late.
- Thread asleep: 522-543 µs late on average, 1.0 ms at worst.

The game's alarms tolerate that (D). They are the speaker update (every 6.67 ms, its schedule absolute so lateness
doesn't accumulate), the 0.5 s GX hang watchdog, and sleeps. Frames are paced by the retrace, not by alarms.

## Kept, with their cost

| choice | cost | why | what would change it |
|---|---|---|---|
| Saturating float-to-int casts (`-fno-strict-float-cast-overflow`) | no difference in scalar code; a vectorised `f32` to `u8` loop 4.7x slower (0.43 against 0.09 ns per value) | the console's `fctiwz` and `__cvt_fp2unsigned` saturate, and game logic may depend on out-of-range results | a profile showing a hot conversion loop: convert that loop explicitly |
| `-fno-strict-aliasing` | H: 0-5% (no type-based alias analysis) | the decompiled code puns types (`agents/petari-audit/PUNNING.md`) | fix the puns, then A/B the frame time |
| `BE<T>` file data | +0.2 ns per four-field record in a load-bound loop (0.56 against 0.36 ns) | data the files and GPU define keeps the console's byte order | a profile showing collision or animation readers hot: convert that format once at load |
| One simulation thread for all game threads | none against the console's model | the game uses `OSDisableInterrupts` as a lock throughout | n/a. Parallel work is the host's: renderer, audio, input, disc |
| Command ring, 16 MB | 16 MB of memory | the game writes far less than that between flushes; more aborts with a message | a frame that needs more |
| Spans end only at flushes | the host gets a frame's commands in a few spans | spans never split a command | finer spans if the renderer wants to start earlier |

## Planned, measured before adopted (H)

- **ThinLTO with whole-program devirtualisation.** Inlines across units (`OSRestoreInterrupts`, `GXBegin`, the `MR::`
  helpers) and devirtualises `LiveActor` calls. Measure the frame time on the galaxy tour route.
- **Profile-guided optimisation.** Train on the galaxy tour harness.
- **Two module builds:** x86-64 baseline and x86-64-v3 (AVX2, FMA, MOVBE: the byte swap folds into the load). The host
  loads the best one the CPU runs.
- **MTX/VEC library.** Native SSE/NEON code where the console's was paired-single assembly.
- **Linux huge pages for the arena** (`MADV_HUGEPAGE`). Measure TLB misses.
- **Vertex batching in the heaviest immediate-mode loops** (ocean rings, water, flags). One reservation per vertex is
  1.0 ns against 1.4 ns per call (M); worthwhile only where a profile shows these loops.
- **Renderer parses ring spans in place.** v1.0.1's renderer copies each frame's FIFO into its own buffer; that copy
  goes with the host integration.

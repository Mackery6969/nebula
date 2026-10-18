# v2 features: where they are (2026-10-17)

Saved before the move to Petari as the game's core. Nothing here is pushed. `v2-features` and `ray-tracing`
point at the same commit: the features below were built on `wt-motion` together with ray tracing and share files
with it, so they are not separable by commit. `GALAXY_RAY_TRACING=0` (or the setting) runs all of them without
ray tracing.

## On `v2-features` (release-1.0.1 + 252 commits)

| Area | What | Main commits |
|---|---|---|
| Frame interpolation | Sub-frames between game frames, evaluated on the GPU from a recorded replay; layers (scene, screen, mirror pass) move separately; particles per particle; game-side motion signals (module generation 20); sub-frames yield when the game misses VIs; temporal upscaling of every presented frame | bb11a7f, d39e81a, c081a75, ab327d0, 8a8e099, 2ba118c, 6cc9461, a87f0b6 |
| DLSS super resolution | Through the user's Streamline install (downloaded from Setup, launcher or the in-game menu) | ac62caf, e3e3c14, 17c2ab4, 7980ca9, a57fb9b |
| DLSS Frame Generation | Streamline DLSS-G with Reflex and latency markers, HUD-less picture, launcher choice 2x/3x/4x | bc0bcf8, 42d0624, fb71efc |
| FSR 3 | With interpolation by default | 511da6d |
| Anti-aliasing | SMAA and FXAA on the 3D scene before the 2D layer | f303419, 2f391a6 |
| Enhanced lighting | The game's lights and materials per pixel in linear light | 6c981f4 and 9 more |
| GPU choice | Choose the GPU; DLSS and ray tracing follow it; fixed for the session | 0d8c055, 4d69600, cc9bbb3 |
| Settings and launcher | Settings menu off the render thread's lock, galaxy select for testing, text speed 1x-3x, save export and import, SYSTEM page | 57aaffa, a1597b3, 960ad62, 37e8797, 2b9433a |
| Audio | Wii Remote speaker sounds | 4382624 |
| Robustness | UTF-8 code page paths, NAND rebuilds, output relay never stalls, crash reports with the guest call chain, VI cadence waits | 745647c, 5144a58, 282da24, adf08a3, 14546e9, 5d4047f |
| Textures | The game's one-texel checker dither decodes flat | 10a371a, 97b3191 |
| Test hooks | Galaxy tour of every scenario, teleports, Grand Star power | 45082c9, f7d0311 |
| Ray tracing | See `RAY_TRACING_STATUS.md` | |

## On other branches (commits not in `v2-features`)

Some may already be in `v2-features` under rebased commits; check with `git cherry` before merging.

| Branch | Ahead | What |
|---|---|---|
| wt-lowend | 20 | Low-end performance: no cursor calls in the input poll, resident vertices in video memory, Match Output at screen pixels, display-list validation by published writes, disc pages released, profiler fixes; the last commit (68d0fcf) is unverified WIP (newest-XFB present, event waits) |
| pr-23-refined | 4 | Wine/Proton: NAND named-stream layout chosen from the volume; Windows unchanged |
| pr-23 | 2 | The contributor's PR #23 as submitted |
| texture-packs | 9 | Texture packs (chosen folder, atomic dumps, preload report) |
| checklist, checklist-d, checklist-h | 14, 12, 21 | 1.0 checklist work: interpolation boxes, controllers, material-table tests |
| wt-controllers | 5 | Controller docs, PlayStation hints, scan timers |
| wt-audio | 3 | Late AI completions held for the game's audio frame |
| hotfix-1.0.1, crash-bp63-hotfix, issues-1.0.0 | 3, 5, 2 | Released or superseded 1.0.x fixes |
| enhanced-2, render-fixes, rt-* | 1-5 | Superseded experiments |

## Relation to the Petari core

Everything that works on the GX command stream and the presented picture (interpolation, DLSS, FSR, AA,
enhanced lighting, ray tracing, the settings) carries over if the native game keeps drawing through GX into the
same backend. Game-side hooks found by address (motion signals, sun site hook, text speed) become calls in the
game's own source.

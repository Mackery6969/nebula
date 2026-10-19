# Ray tracing audit (native build, 2026-10-19)

Every pass of the ray tracer, classed by where its data comes from and whether its maths is physical.
**Native**: from the game's own objects or draw state. **Screen**: read back from the rasterised picture.
**Art**: hand-set constants standing in for physics. Board: claude.ai/artifact/5QdgTtKhqPvCuYukorJqY7.

## Inputs

| Input | Today | Target |
|---|---|---|
| Geometry | Native: every opaque scene draw's eye-space triangles, rebuilt each frame (ray_tracing.hlsl GeometryCS) | Done; off-screen actors kept drawn for shadows (row 34); for reflections and emitters still clipped |
| Primary surface | Native: camera ray matched to the raster depth, Hanika 2021 terminator lift | Done |
| Hit material | Native: the draw's TEV program at the hit (rt_tev.hlsli), two texture slots per triangle from the game's own texture generators | Indirect stages; reflections still texture x colour (rt_reflections.hlsl hit_shade) |
| Alpha test | Native: the draw's TEV alpha and alpha compare (was screen: card rule, GetDimensions on a root descriptor read 0) | Done (row 43) |
| Sun | Native: the stage's key light (LightDirector area light), the real disc (0.533 deg) | Done |
| Sky | Native: the game's sky lists traced (SkyProbeCS), level = the stage's ambient | Mirror-sharp sky in reflections |
| Point lights | Native positions/colours (LightPointCtrl, requesters); **Art** falloff windowed to 0 at a reach | Inverse square, no reach limit |
| Emitters | **Screen**: bright picture pixels (SurfaceEmitCS), glow sprites' added light (GlowEmitCS); **Art** power in "Luma" units, caps 0.2 / 0.5, size curve; temporal memory with fade rates and identity matching | Every emissive draw's triangles as area lights, radiance from its TEV output, flux pi L A; importance-sampled with shadow rays (ReSTIR DI) |
| Bounce light | Native under physical (physical_hit: TEV albedo x sun with shadow ray + sky + emission); **Screen** in the art model (previous picture) | Multi-bounce radiance cache |
| Exposure | Physical camera: white facing the player's up at full light | Done; HDR shoulder per channel (composite film_light) to review |

## Baked light (rows 37, 38)

Measured (Beach Bowl dump): 81% of triangles unlit with light in vertex colours or registers; the main map program
`clamp(lerp(C1, C0, v) x 2)` then `(A + colour x texture) x 2` makes fully lit sand 4 C0 x texture, clipped by the
game itself. Today: unlit vertex colour keeps its hue (full light). Target: each program's neutral level (where it
shows its texture 1:1), solved per draw by a CPU copy of the TEV evaluator; lit draws with vertex colours; per-actor
light boosts (LightDirector actor lights, point-light requests) removed under physical light.

## Lighting passes (rt_lighting.hlsl)

| Pass | Class | Finding |
|---|---|---|
| CaptureCS | Screen | Picture capture for the art model's bounce, sky bins, glow differences, star colour. Physical path needs none of it except the glow differences used by GlowEmitCS (to go) |
| SurfaceEmitCS | Screen, Art | Lights from bright pixels at night (`smoothstep` thresholds 0.1-0.4, 0.55-0.85). To go |
| GlowEmitCS / AuthoredEmitCS / EmitMergeCS | Screen, Art | See Emitters. Clustering, remembered lights, fades: to go |
| TraceCS sun | Native, physical (N.L, real disc, shadow rays) | Foliage self-shadow skip under 60 units (Art) to remove once alpha tests are exact (done) |
| TraceCS points / glows | Art | Strongest 1-4 lights shadow-tested, the rest take a confidence-blended visibility (rank_confidence); reach windows. Replace with resampled importance sampling (Bitterli 2020) and per-sample shadow rays |
| TraceCS bounce | Physical under kPhysicalFlag | One bounce; hits shaded by TEV albedo, sun, sky, table emission |
| TemporalCS / SpatialA/M/B | Partly physical | Penumbra filter by blocker distance (SIGMA-like); lighting at sample resolution (g_factor), upsampled: contact detail lost. Review against NRD SIGMA/ReLAX; no history clamping audit yet |
| SpatialBCS art model | Art | kDayShadow, kDayForm, kDayOcclusion, kDayTint, kDayLevel, night model: bypassed under physical |
| ReferenceCS | Physical | Ground truth: physical path tracer, same units and exposure |
| CompositePS | Physical (film path) | lin(colour) x light, hue-kept shoulder |

## Reflections (rt_reflections.hlsl), post (rt_post.hlsl)

Not yet audited line by line. Known: on-screen hits take the previous picture's colour (Screen); off-screen hits the
art model's hit_shade; water replaces the surface with a mirror (waves lost, row 41); flashes on Battlerock metal
(row 20). Target: hits shaded as physical_hit (TEV albedo + sun + sky + emitters), Fresnel-weighted over the game's own
surface colour, normals from the game's own wave and normal maps.

## Order

1. Emitters native (rows 14, 16, 16b, 16c): emissive triangles from TEV, sampled with shadow rays; off-screen emitters
   kept drawn while their light reaches the view; no reach limit.
2. De-bake neutral level (rows 37, 38); lit vertex colours; actor light boosts.
3. Reflections and water physical (rows 12, 18, 20-23, 41).
4. Denoising and stability (rows 10, 11, 19, 32): full-resolution shadow edges, history audit, flicker metric in motion.
5. Settings: per-feature switches and Low/Medium/High (row 40); every resolution and aspect (row 42).
6. Cost (row 25).

Every step: the physical reference path tracer as ground truth, before/after pictures on the board, the 5-view bench
(port/tools/rt_bench.sh, rt_score.py), the pixel probe (GALAXY_RT_PROBE) for any pixel in question.

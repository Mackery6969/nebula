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
| Sun | Native: the stage's key light, the real disc (0.533 deg); optionally fitted to the baked light (GALAXY_RT_FIT_LIGHT=1, below) | Per-area lights |
| Sky | Native: the game's sky lists traced (SkyProbeCS), level = the stage's ambient | Mirror-sharp sky in reflections |
| Point lights | Native positions/colours (LightPointCtrl, requesters); physical: the game's own falloff h^2 / (h^2 + d^2) (inverse square past h), no reach window | Sphere emitters of their objects' size |
| Emitters | Physical: the table's light sources (lava, Lumas, ? blocks, star bits) are area lights of their own triangles: radiance their TEV output (linear), power pi L A (two-sided twice); 2/4/8 candidates by power, one kept by resampled importance (Talbot 2005, Bitterli et al. 2020), one shadow ray (emitter_light). Glow sprites: still **Screen**/**Art** (GlowEmitCS) | Glows as luminous quads; spatiotemporal reuse (ReSTIR) |
| Bounce light | Native under physical (physical_hit: TEV albedo x sun with shadow ray + sky + emission); **Screen** in the art model (previous picture) | Multi-bounce radiance cache |
| Light units | Native: the stage's ambient and key as the game sets them, linear (white 1); sky = ambient, sun = full - ambient | Done (was each channel divided by the full light: every stage white and day-bright) |
| Exposure | Physical camera set for the game's white light: a white surface facing the player's up shows white, the sun at no less than 30 degrees (sunny-16). A stage lit brighter (GX's doubled light) is exposed down to it; a dimmer one keeps half its dimness in log luminance (partial adaptation, brightness about the square root of luminance: Stevens 1957) | HDR shoulder per channel (composite film_light) to review |
| Stage light | Every galaxy measured reports the default planet light (ambient 0.251, white key at 700000, 700000, 700000); the art's light is baked into vertex colours. Fitted instead (BakedFitCS, fit_baked_light): per visible de-baked sample the baked light (TEV output with the baked vertex colours over the output at the neutral light, grey texture, linear) against its world normal, least squares over nine spherical harmonics (Ramamoorthi & Hanrahan 2001; inverse lighting, Marschner & Greenberg 1997), accumulated over about three seconds, reset with the stage; the sun the first band (Sloan 2008), the sky the rest. Off by default: faithful to the baked light, it set the sun near overhead with a strong fill and looked flat (user, sweep4/5 Good Egg: "muted, no lighting at all"); the key's low sun gives the shape. Colours stay the key's and the stage ambient's (coloured registers bias the measured hue; the traced sky's colour is a backdrop's). A fitted sun below the player's horizon (light baked from local lamps onto undersides: the Observatory) leaves the game's light | Per-area fits; lit models in the fit |
| Sky light | Image-based: the backdrop is drawn as it shows, so its picture is the radiance it sends, in its colours and over its directions (SkyProbeCS map, SH9 irradiance). The stage's ambient says how much a surface open to the sky gets: what the backdrop sends short of it fills from every direction in the ambient's colour; a brighter backdrop is taken down to it (physical_sky_own, physical_sky_fill). A dark space backdrop lights little (raised to the ambient, Good Egg's deep blue tinted its stone lavender); a glowing one lights in its own colour (Melty Molten's lava horizon: grey under the ambient's colour alone, red as liked in expo1) | Done |

## Baked light (rows 37, 38)

Measured (Beach Bowl dump): 81% of triangles unlit with light in vertex colours or registers; the main map program
`clamp(lerp(C1, C0, v) x 2)` then `(A + colour x texture) x 2` makes fully lit sand 4 C0 x texture, clipped by the
game itself.

- Vertex colours and channel light: each program's neutral level (where it shows its texture 1:1), solved per draw by
  a CPU copy of the TEV evaluator (rt_tev_eval.cpp); unlit vertex colours keep their hue at that level, lit channels
  are lit at it. Done (raster and hits).
- Light painted into textures (bevels, groove shadows, metal sheen): `rt_delight.hlsl`. Retinex with chromaticity as
  the reflectance cue (Land & McCann 1971; Funt, Drew & Brockington 1992; Grosse et al. 2009) and a bilateral base
  layer (Durand & Dorsey 2002): a texel's level is the mean log luminance around it (Gaussian, 12% of the texture)
  weighted by likeness in chromaticity (0.05) and log luminance (0.8); its shading is its log luminance less that
  level. One R8_SNORM map per texture-pack key with mips, made on the GPU the first scene a traced draw shows the
  texture as its material (about 125 maps, 3.4 MB on Good Egg), under the draw's own addressing.
  GBufferCS samples it at the hit's coordinates with a ray-cone level (Akenine-Moller et al. 2021). The composite
  divides the pixel's colour by exp(shading). The shading as height bends the normal the per-pixel sun uses (surface
  gradient, Mikkelsen 2020); the height saturates at +-0.35 log units and the slope at 20 degrees, since painted light
  says little of shape. Hits divide their texel likewise.
  Not de-lit: water, clear crystal and glass, sky, effects, light sources, and table rows marked `painted` (skin by
  default: eyes, mouths, brows). Limit: brightness changes at one chromaticity (wood grain, outlines) count as
  shading and become shallow relief. GALAXY_RT_DELIGHT=0 off, GALAXY_RT_DELIGHT_RELIEF (texels per log unit, 1; 2 made
  grass and every texture too rough, user).
- Lit models: their channel at its neutral light keeps the hue of the game's ambient + lights; their vertex colours
  are their material (the game lights them) and stay as painted.
- Vertex colours hold baked light only on the terrain (ClippedMapParts, Planet, PlanetLow, Environment,
  IndirectPlanet, the draw types' lists). Actors (player, NPCs, enemies, rides, crystals, items: MR::DrawBufferType
  0x10-0x16, 0x1B, 0x1C, 0x1F-0x21, 0x24) are lit as they move and paint their vertices with their own colours; map
  objects (houses, toys, blocks, props) likewise: never de-baked (each vertex colour divided by its brightest channel
  turned Toy Time's toy's black hair white).
- Per model (its CP colour arrays), the first frame it draws: its sampled vertices' colours against their normals
  (object space) give its baked light as nine spherical harmonics a channel (least squares, Cholesky with a 1e-3
  ridge; Ramamoorthi & Hanrahan 2001: a distant light's irradiance lies almost wholly in bands 0-2). The vertex shaders
  and the traced triangles' colours (GeometryCS) scale each de-baked colour by the fit at the player's up (taken into
  the model's space by its position matrix; the exposure's reference: white facing the player's up shows white; at
  most the most lit sampled facing's) over the fit at its own normal (1/4 to 4x): the light's fall-off and colour come
  out, what the facing does not explain (paint, occlusion) stays. To the most lit facing alone, Good Egg's hand-shaded
  stone path (fit 1.03, its upward faces 0.52-0.77) went white. Measured (GALAXY_RT_DEBAKE_LOG): Toy Time's robot r2 0.04 (its grille's
  808080 / FFFFFF stripes are paint: each colour divided by its brightest channel had turned them white), hand-shaded
  models up to 0.85, Good Egg's terrain 0.06-0.97 (large planets 0.2-0.3: the rest is occlusion and placed shadow).
  The first frame draws the model as the game paints it. GALAXY_RT_DEBAKE_FIT=0: each colour over its brightest
  channel at the program's neutral level where its distinct colours number more than 16 (paint: 2-12 exact values).
- Per-actor light boosts (LightDirector actor lights, point-light requests) under physical light: open.

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
art model's hit_shade; flashes on Battlerock metal.
Painted reflections: a TEV stage whose texture coordinates the normals generate is an environment map, the game's
painted reflection (RtTevStage.pad[0], header bit 7). With traced reflections on, GBufferCS evaluates the pixel's
material with and without those stages and the composite keeps the share without them (pixel record x bits 24-31); the
traced reflection replaces them at full strength (GX metal class strength 1, was 0.5 over the painted one).
GALAXY_RT_PAINTED_REFLECTION=1 keeps them. Metal only (the draw's class): crystal, glass, glossy and unlit surfaces keep
theirs as their look (Gold Leaf's star outline, an environment map alone, went black). A metal never counts as an
emitter in the reflection pass (a bright metal is bright by what it reflects), and its rays that leave the scene see
at least its own environment map, the probe the game made for it (Space Junk's chrome mirrored black space).
Metal is never too rough to mirror its surroundings: roughness at most 0.58 (the table's 32 metals at 0.78 reflected
nothing past the 0.65 cut-off). Clear crystal, gems and glass: a diamond's F0 (n 2.42: 0.17) and the physical split
colour x (1 - F) + reflection x F (was F0 0.45 at cover 0.8: Melty Molten's and Freezeflame's crystals showed mostly
their dark surroundings).
Falling water (the table's `waterfall`: Honeyhive's and Beach Bowl's falls): a turbulent sheet, its own foam and
streaks lit, sun glints, no mirror image (as glass Honeyhive's showed 80 % of its dark surroundings).
Water (row 41): the game's own waves bend the reflected ray (water_wave): the wave map GX moves the water's texture by
(its indirect texture, alpha and blue the s and t offsets), read at the pixel's footprint level as slopes, its full
range two standard deviations of the sea's slope at a light breeze (Cox & Munk 1954, sigma 0.17 at 5 m/s), turned by
the indirect matrix onto the water texture's axes. Water without a wave map keeps the procedural waves. Target: hits shaded as physical_hit (TEV albedo + sun + sky + emitters), Fresnel-weighted over the game's own
surface colour, normals from the game's own wave and normal maps.

## Emitters by region

The table's `region` (u0,v0,u1,v1 of the material texture) limits an emitter to part of its texture: TEV record pad[2]
bits 8-31; EmitTriangleCS gives a triangle outside it no light (at its centre), TraceCS marks a primary sample outside
it (g_primary x bit 31) so SpatialBCS lights it as any surface. Gusty Garden's spin-bulb plants (FluffLeaf: leaf in
v < 0.5, bulb in v >= 0.5): the bulbs glow at twice their colour, the leaves do not.

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

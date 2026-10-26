# Online co-op plan

Goal: several players in the same galaxy over the internet, each with their own Mario (or Luigi), in the spirit of
[sm64coopdx](https://github.com/Mackery6969/sm64coopdx). Co-op is online only: each player runs Nebula on their own
PC, and games are self-hosted (see [Hosting](#hosting)), with no central service run by the project. This document
records what the codebase gives us today, the approach chosen, and the order of work. It is a plan, not a description
of shipped features.

## What we start from

| Area | Today | Consequence for co-op |
|---|---|---|
| Player | One `MarioActor`, held by `MarioHolder`, reached through `MarioAccess::getPlayerActor()` and `MR::getPlayer*` | Every enemy, object, camera and area check assumes one player |
| Player call sites (`native/petari/src`) | 616 `MR::getPlayer*` (308 of them `getPlayerPos`), 504 `isPlayer`, 208 `isNearPlayer`, 187 `MarioAccess`, 42 files using `MarioActor*` | A second *real* `MarioActor` is a long refactor; remote players must start as something lighter |
| Input | Each Nebula instance drives one Wii Remote, channel 0 (`native/host/host_main.cpp`, `gModule.wpad_report(0, ...)`) | Nothing to change: every player controls the local Mario on their own PC with their own controls |
| Co-Star | Removed. The native WPAD layer (`native/sdk/wpad/wpad_native.c`) ignores the host on channels 1-3, so a second player's star pointer never connects | The game's 2P code (pointer assists, 2P jumps, `Manual2P`, 2P guidance) stays in Petari but can't run. Online co-op replaces it |
| Simulation | The game's threads run on one host thread under the SDK's scheduler (`docs/ARCHITECTURE.md`) | Same-frame state is easy to read and write from one place; a network tick fits next to `nebula_game_vi_retrace` |
| Host/game boundary | Plain C interface, `native/include/nebula_game_api.h` (version 31) | Netcode lives in the host; the game module gets a small set of new `nebula_game_*` / `NebulaHostServices` entries |
| Networking | None | Sockets, the protocol, the lobby and the UI are all new |

## Approach: state sync with remote player actors (sm64coopdx's model)

sm64coopdx doesn't run lockstep. Each client simulates its own Mario fully, sends its state (position, rotation,
animation, action, health) several times a second, and draws the other players as remote Marios driven by that state.
Shared things (coins, stars, enemies, switches) are synced with per-object packets and a host that settles conflicts.

We use the same model, for these reasons:

- Lockstep or rollback needs a bit-identical simulation on every machine. Nebula can't promise that across
  compilers, CPUs, frame interpolation and asynchronous disc reads.
- The local Mario stays exactly as Petari wrote it. Nothing in `Game/Player` changes to get the first version working.
- sm64coopdx shows the model holds up in a 3D Mario game with dozens of players.

## Hosting

Games are self-hosted. There is no lobby server, relay or account system run by the project.

- **Listen server (first).** One player chooses Host in Nebula, which opens a UDP port (configurable, default to be
  picked). The other players choose Join and enter `address:port`. The hosting player's game is the authoritative
  world (see below). This is how sm64coopdx's direct connections work, without its CoopNet relay.
- **Reaching the host.** The host forwards the UDP port on their router, or the group uses a VPN overlay such as
  Tailscale, ZeroTier or Radmin VPN and joins by its address. The docs explain both; Nebula itself does no NAT
  traversal.
- **Dedicated server (later).** The world simulation *is* the game, so a server with nobody playing on it needs a
  headless Nebula: the game module running with no window, renderer or audio output, and no local Mario (or a hidden
  one). That's a real but separate project after Phase 2. The protocol is designed so the authority doesn't have to be
  a player, so a dedicated server can slot in later without changing clients.
- **Server settings.** Port, maximum players, password, player-to-player knockback/damage on or off, and whose save
  is used (the host's, as in sm64coopdx). They live in a `[coop]` section of the settings file so a dedicated server
  can be configured without a menu.

## Remote players

A `RemotePlayer` is a `LiveActor` that loads Mario's or Luigi's model and animations (`MarioAnimator`'s resources) and
plays whatever animation the network names, at the position, rotation and gravity it received, interpolated between
packets. It has no physics or state machine of its own. Later phases give it collision with the local Mario and with
enemies.

## Shared world state

The host is authoritative. Rough order of what to sync, from cheapest to hardest:

1. Stage and scenario changes. Everyone follows the host's galaxy and star. This is sm64coopdx's
   `packet_change_level`.
2. Collected stars, Grand Stars, star bits, coins, 1-Ups and save flags (`GameDataFunction`). Comparable to
   `packet_collect_*` and `packet_save_set_flag`.
3. Object deaths and state, through a sync-object registry keyed by placement ID (`JMapInfo` link IDs), like
   `sync_object.c`/`packet_object.c`.
4. Bosses and scripted demos. These are the hardest: `MR::getPlayerDemoActor` and cutscenes assume one player. One
   way through is that the player who starts a demo plays it and the others wait for it to end.

## Phases

Each phase ships on its own and leaves the game playable.

### Phase 0: groundwork (no networking)

- [ ] Add the `[coop]` settings section and `--host [port]` / `--join address:port` command-line options, hidden
      until the feature is real.
- [ ] Add an inventory script (in the style of `native/tools/*.py`) that lists every single-player assumption
      (`MR::getPlayer*`, `isPlayer`, `MarioAccess`, `getPlayerDemoActor`) by subsystem, so later phases can tick
      them off.
- [ ] Add game API entry points (bumping `NEBULA_GAME_API_VERSION`): read the local player's snapshot (position,
      rotation, gravity, animation name and frame, action/nerve, health) once per frame, and spawn, move or remove
      remote player actors.

### Phase 1: ghosts

- [ ] Socket layer in `runtime/src/net/` (UDP, Windows and POSIX, the SDL host included) with reliable/ordered
      channels on top. sm64coopdx's `packet_reliable.c` / `packet_ordered.c` are a reference for the design.
- [ ] Host and Join (`address:port`) in Nebula's settings menu and on the command line; an optional password.
- [ ] Each client sends its player snapshot at 30 Hz. Others draw it as a `RemotePlayer` with no collision
      (a "ghost").
- [ ] Each player's name over their head; a player list in Nebula's settings menu.
- [ ] Version and `main.dol` hash handshake. Clients with a different build or different mods are refused.

### Phase 2: same world

- [ ] The host's stage and scenario changes pull everyone along.
- [ ] Collectibles and save flags sync, with the host authoritative.
- [ ] Remote players stomp, spin and touch objects. Hits are sent as events to the client that owns the object.

### Phase 3: shared enemies and objects

- [ ] Sync-object registry and per-object packets. Begin with simple enemies (Goomba/Kuribo, Octoomba), then platforms
      and switches.
- [ ] Each enemy targets the *nearest* player instead of `MR::getPlayerPos`. This is where the call-site inventory
      gets used: route `MR::getPlayer*` through a "player relevant to this actor" helper, one subsystem at a time.

### Phase 4: bosses, demos, polish

- [ ] Demos/cutscenes: whoever triggers one plays it, everyone else is frozen or faded out.
- [ ] Camera and HUD per player; spectate on death.
- [ ] Chat, Discord presence for the lobby (`discord_presence.cpp` already exists), player colours.

## Licensing

Nebula is GPLv3 and Petari is CC0. The sm64coopdx repository has no license file, so its code can't be copied in.
Use it as a reference for protocol and design ideas only, and write Nebula's netcode from scratch.

## Open questions

- Transport: raw UDP with our own reliability layer, or an existing library (ENet, GameNetworkingSockets, both
  permissive) vendored under `third_party/`?
- Saves: the host's save is the session's (as in sm64coopdx). Do joining players' own saves stay untouched, or do
  they get a copy of what they collected?
- Android: should the SDL host get co-op from Phase 1, or later?

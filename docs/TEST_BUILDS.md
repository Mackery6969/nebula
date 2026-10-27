# Test builds

Every pull request and every push to `main` builds Nebula for Windows, Linux and Android and keeps the result as a
download for 14 days (`.github/workflows/packages.yml`). These are test builds, not releases: they have no installer,
no launcher, no update check and no ray-tracing material tables, and the Android app is signed with a debug key.
macOS and iOS have no build yet.

To download one, open the pull request's **Checks** tab (or the run under **Actions** > **Packages**) and pick an
artifact under **Artifacts**. You need to be signed in to GitHub.

| Artifact | Contents |
|---|---|
| `nebula-windows-x64` | `NebulaNative.exe` (Direct3D 12), `NebulaNativeVk.exe` (Vulkan), `smg_game.dll`, `nebula-import.exe` |
| `nebula-linux-x64` | `NebulaNative` (Vulkan, SDL3), `libsmg_game.so`, `libSDL3.so.0`, `nebula-import` |
| `nebula-android` | `Nebula.apk` (arm64 and x86_64) |

## Windows and Linux

1. Pack your own disc image of the North American Super Mario Galaxy (`RMGE01`) once. It stays on your PC:

   ```
   nebula-import identify SMG.rvz
   nebula-import import SMG.rvz --output content
   ```

2. Run the game from a terminal, so its log shows:

   ```
   NebulaNative --pak content/game.pak --nand saves
   ```

   `saves` is created if it's missing and holds the save files. On Linux the folder must also hold `libSDL3.so.0` and
   `libsmg_game.so` (they're found next to the program), and you need a Vulkan driver. Nebula's in-game settings menu
   (F1) is Windows-only for now: on Linux, settings come from `runtime_settings.ini` and `config.ini`.

The main menu's Host opens the Host page: pick a save with left and right (or New save), set the room, then press
Host. Saves are folders under `saves/coop_saves/`, each with its own achievements; on the first start the files of
your old save become saves named `FILE 1`, `FILE 2` and so on, and the old save is left as it was.

Online co-op lines in the log start with `[coop]`. `--host <port>` and `--join <address[:port]>` are accepted but don't
connect to anything yet (docs/COOP_PLAN.md).

## Android

Install `Nebula.apk` (allow installs from unknown sources), open it, and pick your disc image in its setup. A test
build only installs over another test build: uninstall a release first, or the other way round.

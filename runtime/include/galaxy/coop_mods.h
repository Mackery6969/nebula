// Online co-op's room check and shared mods (docs/COOP_PLAN.md, "Mods: the room's, not the player's"): what a joiner
// must play like to be let in (the same disc, the room's mods), and what moves the room's mods from the host to a
// joiner. In the spirit of sm64coopdx's mod list and downloads (packet_mod_list.c, packet_download.c), written for
// Nebula:
//
// - The disc: boot.bin's game id and revision and a hash of main.dol and the file table, so another release or a
//   changed image is told apart.
// - The room's mods: the host's mods that change the game (Riivolution, game files, loose files), in their order, with
//   their choices. Texture packs and HD movies stay each player's own, as sm64coopdx's DynOS packs do.
// - A mod is known by its id (its folder's name) and a hash of its files, so a joiner's copy is used when it is the
//   same, and a downloaded one is checked before it is used. Downloads wait in a cache (coop_mods/ beside the
//   settings, a folder per hash), apart from the player's own mods.
#pragma once

#include "galaxy/net/bytes.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace galaxy::coop {

// A mod past these isn't shared (a room's whole list, one mod, one of its files, how many files it has).
inline constexpr std::uint64_t kMaxRoomModBytes = std::uint64_t{4} << 30;
inline constexpr std::uint64_t kMaxModBytes = std::uint64_t{2} << 30;
inline constexpr std::size_t kMaxModFiles = 20000u;
inline constexpr std::size_t kMaxRoomMods = 64u;
inline constexpr std::size_t kMaxModPath = 512u;

// XXH64, fed a piece at a time: the hash of a disc or a mod's files.
class ContentHash {
public:
    explicit ContentHash(std::uint64_t seed = 0u);
    void update(std::span<const std::uint8_t> data);
    void update(std::string_view text);
    void update_u64(std::uint64_t value);
    [[nodiscard]] std::uint64_t finish() const;

private:
    std::uint64_t v_[4];
    std::uint8_t buffer_[32];
    std::size_t buffered_ = 0u;
    std::uint64_t total_ = 0u;
    std::uint64_t seed_ = 0u;
};

// The game's disc as a room compares it.
struct DiscInfo {
    std::string game_id;  // boot.bin's six characters (RMGE01)
    unsigned revision = 0u;
    std::uint64_t hash = 0u;  // main.dol's and the file table's

    friend bool operator==(const DiscInfo&, const DiscInfo&) = default;
};
// "RMGE01 REVISION 0" for messages.
[[nodiscard]] std::string disc_text(const DiscInfo& disc);
// The disc's info from boot.bin's first bytes, main.dol and the file table.
[[nodiscard]] DiscInfo describe_disc(std::span<const std::uint8_t> boot, std::span<const std::uint8_t> dol,
                                     std::span<const std::uint8_t> fst);

// One of a mod's files: its path in the mod's folder ('/' between folders) and its size.
struct ModFile {
    std::string path;
    std::uint64_t size = 0u;

    friend bool operator==(const ModFile&, const ModFile&) = default;
};

// A mod as a room plays it.
struct SharedMod {
    std::string id;           // the host's folder name
    std::uint64_t hash = 0u;  // its files' (hash_mod_files)
    bool own_text = true;     // mods.h's ModInfo::own_text
    std::vector<std::pair<std::string, unsigned>> options;  // Riivolution choices: key, choice (0 off)
    std::vector<ModFile> files;
    std::filesystem::path folder;  // this player's copy (not sent)

    [[nodiscard]] std::uint64_t size() const;
    // The same mod played the same way: id, files and choices.
    [[nodiscard]] bool plays_like(const SharedMod& other) const;
};

// Both lists play the same mods in the same order the same way.
[[nodiscard]] bool same_mods(std::span<const SharedMod> a, std::span<const SharedMod> b);
// The mods' ids, ", " between them ("NONE" for no mod), for messages.
[[nodiscard]] std::string mods_text(std::span<const SharedMod> mods);

// A path a mod's file may have: relative, '/' between non-empty names, none of them "." or "..", no '\\' or ':'.
[[nodiscard]] bool safe_mod_path(std::string_view path);
// The files of a mod's folder, by path; none when it can't be read or is past the limits above.
[[nodiscard]] std::optional<std::vector<ModFile>> list_mod_files(const std::filesystem::path& folder);
// The hash of a mod's files (their paths, sizes and bytes, in the list's order); none when one can't be read whole.
// Remembered for a folder whose files haven't changed (sizes and times), so a large mod is read once per run.
[[nodiscard]] std::optional<std::uint64_t> hash_mod_files(const std::filesystem::path& folder,
                                                          std::span<const ModFile> files);
// `mod.files` and `mod.hash` from `mod.folder`. False when the folder can't be read.
bool describe_mod(SharedMod& mod);
// Where one of a mod's files is in its folder here.
[[nodiscard]] std::filesystem::path mod_file_path(const std::filesystem::path& folder, std::string_view path);

// A list of mods as a message has it: with their files or without them.
void write_mods(net::ByteWriter& out, std::span<const SharedMod> mods, bool files);
// Read back; none when malformed, unsafe or past the limits.
[[nodiscard]] std::optional<std::vector<SharedMod>> read_mods(net::ByteReader& in, bool files);

// The cache of downloaded mods: a folder per hash under `root`.
class ModCache {
public:
    explicit ModCache(std::filesystem::path root) : root_(std::move(root)) {}
    [[nodiscard]] const std::filesystem::path& root() const noexcept { return root_; }
    // A downloaded copy of the mod (its hash's folder, its files as listed); none when there isn't one.
    [[nodiscard]] std::optional<std::filesystem::path> find(const SharedMod& mod) const;
    // Where a mod downloads before it is checked (emptied first). Empty when it can't be made.
    [[nodiscard]] std::filesystem::path begin(const SharedMod& mod) const;
    // The download checked against the mod's hash and moved to its folder: that folder, or none (the download
    // removed) when it isn't the mod.
    [[nodiscard]] std::optional<std::filesystem::path> finish(const SharedMod& mod) const;

private:
    [[nodiscard]] std::filesystem::path folder(const SharedMod& mod) const;
    [[nodiscard]] std::filesystem::path part(const SharedMod& mod) const;

    std::filesystem::path root_;
};

// A join that waits for Nebula to start again with the room's mods: where to, and the mods (their folders here).
struct NextJoin {
    std::string address;
    std::vector<SharedMod> mods;
};
bool write_next_join(const std::filesystem::path& file, const NextJoin& join);
// The join written, once (the file is removed); none when there is none or it can't be read.
[[nodiscard]] std::optional<NextJoin> take_next_join(const std::filesystem::path& file);

}  // namespace galaxy::coop

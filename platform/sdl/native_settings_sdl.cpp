// SPDX-License-Identifier: GPL-3.0-only
// Nebula's in-game settings menu on SDL3 platforms: the Windows window's model (runtime/src/gx/native_settings_*.inl),
// built here with what the SDL host has in place of Win32's. The game draws the menu (NebulaSettingsMenu); the host's
// own overlay drawing stays Windows-only.

#include "sdl_host.h"

#include <SDL3/SDL.h>

#include "galaxy/controller_hid.h"
#include "galaxy/coop_menu.h"
#include "galaxy/gx/display_modes.h"
#include "galaxy/gx/galaxy_select.h"
#include "galaxy/gx/host_window.h"
#include "galaxy/gx/post_process_settings.h"
#include "galaxy/gx/render_config.h"
#include "galaxy/gx/renderer_common.h"
#include "galaxy/gx/texture_pack.h"
#include "galaxy/gx_d3d12.h"
#include "galaxy/host_pointer_events.h"
#include "galaxy/mii_import.h"
#include "galaxy/mod_import.h"
#include "galaxy/mods.h"
#include "galaxy/native_host.h"
#include "galaxy/native_settings_pad.h"
#include "galaxy/platform_time.h"
#include "galaxy/runtime_settings.h"
#include "galaxy/virtual_keys.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace galaxy::gx {

// The monitors (display_modes.h) as SDL's displays: a display's number stands for its GDI device name.

std::string narrow_utf8(std::wstring_view text) {
    std::string out;
    for (const wchar_t unit : text) {
        const auto code = static_cast<std::uint32_t>(unit);
        if (code < 0x80u) {
            out += static_cast<char>(code);
        } else if (code < 0x800u) {
            out += static_cast<char>(0xC0u | code >> 6u);
            out += static_cast<char>(0x80u | (code & 0x3Fu));
        } else if (code < 0x10000u) {
            out += static_cast<char>(0xE0u | code >> 12u);
            out += static_cast<char>(0x80u | (code >> 6u & 0x3Fu));
            out += static_cast<char>(0x80u | (code & 0x3Fu));
        } else {
            out += static_cast<char>(0xF0u | code >> 18u);
            out += static_cast<char>(0x80u | (code >> 12u & 0x3Fu));
            out += static_cast<char>(0x80u | (code >> 6u & 0x3Fu));
            out += static_cast<char>(0x80u | (code & 0x3Fu));
        }
    }
    return out;
}

std::wstring widen_utf8(std::string_view text) {
    std::wstring out;
    for (std::size_t i = 0; i < text.size();) {
        const auto lead = static_cast<unsigned char>(text[i]);
        const std::size_t length = lead < 0x80u ? 1u : lead >> 5u == 6u ? 2u : lead >> 4u == 14u ? 3u : 4u;
        std::uint32_t code = length == 1u ? lead : length == 2u ? lead & 0x1Fu : length == 3u ? lead & 0x0Fu : lead & 0x07u;
        for (std::size_t k = 1; k < length && i + k < text.size(); ++k) {
            code = code << 6u | (static_cast<unsigned char>(text[i + k]) & 0x3Fu);
        }
        out += static_cast<wchar_t>(code);
        i += length;
    }
    return out;
}

std::vector<MonitorInfo> enumerate_monitors() {
    std::vector<MonitorInfo> monitors;
    int count = 0;
    SDL_DisplayID* displays = SDL_GetDisplays(&count);
    if (displays == nullptr) {
        return monitors;
    }
    const SDL_DisplayID primary = SDL_GetPrimaryDisplay();
    for (int i = 0; i < count; ++i) {
        MonitorInfo monitor;
        monitor.device = std::to_wstring(displays[i]);
        if (const char* name = SDL_GetDisplayName(displays[i])) {
            monitor.name = name;
        }
        SDL_Rect rect{};
        if (SDL_GetDisplayBounds(displays[i], &rect)) {
            monitor.bounds = {rect.x, rect.y, rect.x + rect.w, rect.y + rect.h};
        }
        monitor.work_area = SDL_GetDisplayUsableBounds(displays[i], &rect)
                                ? RECT{rect.x, rect.y, rect.x + rect.w, rect.y + rect.h}
                                : monitor.bounds;
        if (const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displays[i])) {
            monitor.current = {static_cast<unsigned>(mode->w), static_cast<unsigned>(mode->h),
                               static_cast<unsigned>(std::lround(mode->refresh_rate))};
        }
        monitor.primary = displays[i] == primary;
        int modes = 0;
        if (SDL_DisplayMode** list = SDL_GetFullscreenDisplayModes(displays[i], &modes)) {
            for (int m = 0; m < modes; ++m) {
                const DisplayModeInfo mode{static_cast<unsigned>(list[m]->w), static_cast<unsigned>(list[m]->h),
                                           static_cast<unsigned>(std::lround(list[m]->refresh_rate))};
                if (std::find(monitor.modes.begin(), monitor.modes.end(), mode) == monitor.modes.end()) {
                    monitor.modes.push_back(mode);
                }
            }
            SDL_free(list);
        }
        monitors.push_back(std::move(monitor));
    }
    SDL_free(displays);
    std::stable_sort(monitors.begin(), monitors.end(), [](const MonitorInfo& a, const MonitorInfo& b) {
        if (a.primary != b.primary) return a.primary;
        if (a.bounds.left != b.bounds.left) return a.bounds.left < b.bounds.left;
        return a.bounds.top < b.bounds.top;
    });
    return monitors;
}

const MonitorInfo* find_monitor(const std::vector<MonitorInfo>& monitors, std::wstring_view device) {
    for (const MonitorInfo& monitor : monitors) {
        if (!device.empty() && monitor.device == device) return &monitor;
    }
    for (const MonitorInfo& monitor : monitors) {
        if (monitor.primary) return &monitor;
    }
    return monitors.empty() ? nullptr : &monitors.front();
}

std::vector<std::pair<unsigned, unsigned>> monitor_resolutions(const MonitorInfo& monitor) {
    std::vector<std::pair<unsigned, unsigned>> out;
    for (const DisplayModeInfo& mode : monitor.modes) {
        const std::pair<unsigned, unsigned> size{mode.width, mode.height};
        if (std::find(out.begin(), out.end(), size) == out.end()) out.push_back(size);
    }
    return out;
}

}  // namespace galaxy::gx

#if defined(__ANDROID__)
// Android takes no mods (mod_import.cpp is not built there): nothing imports and nothing is removed.
namespace galaxy::mods {
bool is_mod_archive(const std::filesystem::path&) { return false; }
ImportResult import_mod(const std::filesystem::path&, const std::filesystem::path&,
                        const std::function<void(const ImportProgress&)>&, const std::atomic<bool>*) {
    ImportResult result;
    result.message = "MODS ARE NOT SUPPORTED HERE";
    return result;
}
bool remove_mod(const std::filesystem::path&, const std::string& id, std::string* message) {
    if (message != nullptr) *message = "CANNOT REMOVE " + id;
    return false;
}
}  // namespace galaxy::mods
#endif

// Joy-Con pairing reads them over Windows HID; SDL pairs nothing (the CONTROLS page has no PAIR row here).
namespace galaxy::host {
JoyConStatus joycon_status() noexcept { return {}; }
void set_joycon_pairing(bool) noexcept {}
}  // namespace galaxy::host

namespace galaxy::gx {

namespace host_window {
std::atomic<std::uintptr_t> settings_window{0};
std::atomic<bool> temporal_interp_suspended{false};
std::atomic<bool> rtx_adapter{false};
extern std::atomic<bool> ray_tracing_offered;
unsigned upscaler_render_scale(const RenderConfig& render, unsigned internal);
void save_native_settings_now();
}  // namespace host_window

using host_window::flush_native_settings_if_dirty;
using host_window::hidden_window_mode_enabled;
using host_window::poll_render_config_file;

namespace {

// Nebula started again in this process's place (gx_d3d12.h request_restart, host_window_sdl.cpp).
bool restart_nebula() { return request_restart(); }

// The desktop's file picker (SDL's, on its thread): `done` gets the paths chosen, none when cancelled.
void pick_files_on_desktop(const char* title, bool folders, std::function<void(std::vector<std::filesystem::path>)> done) {
    (void)title;
    auto* held = new std::function<void(std::vector<std::filesystem::path>)>(std::move(done));
    galaxy::sdl::run_on_sdl_thread([held, folders] {
        const SDL_DialogFileCallback chosen = [](void* user, const char* const* list, int) {
            auto* callback = static_cast<std::function<void(std::vector<std::filesystem::path>)>*>(user);
            std::vector<std::filesystem::path> paths;
            for (; list != nullptr && *list != nullptr; ++list) {
                paths.emplace_back(*list);
            }
            (*callback)(std::move(paths));
            delete callback;
        };
        if (folders) {
            SDL_ShowOpenFolderDialog(chosen, held, nullptr, nullptr, true);
        } else {
            SDL_ShowOpenFileDialog(chosen, held, nullptr, nullptr, 0, nullptr, true);
        }
    });
}

// The desktop's file manager at a folder.
void open_folder_on_desktop(const std::filesystem::path& folder) {
    const std::string url = "file://" + folder.string();
    galaxy::sdl::run_on_sdl_thread([url] { (void)SDL_OpenURL(url.c_str()); });
}

// Streamline (DLSS) is Windows-only: never present here.
namespace streamline {
bool dlss_supported() noexcept { return false; }
bool frame_generation_supported() noexcept { return false; }
unsigned frame_generation_max() noexcept { return 0u; }
bool frame_generation_dynamic() noexcept { return false; }
unsigned frames_presented() noexcept { return 0u; }
std::wstring default_directory() { return {}; }
}  // namespace streamline
unsigned dlss_min_render_width(unsigned, unsigned, unsigned) noexcept { return 0u; }

// The Win32 names the model keeps.
using WPARAM = std::uintptr_t;
using DWORD = std::uint32_t;
using UINT = unsigned;

void* GetCurrentThread() noexcept { return nullptr; }
void SetThreadDescription(void*, const wchar_t*) noexcept {}

std::uint64_t GetTickCount64() noexcept {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::steady_clock::now().time_since_epoch())
                                          .count());
}

#include "../../runtime/src/gx/native_settings_model.inl"

#include "../../runtime/src/gx/native_settings_pages.inl"

}  // namespace

namespace host_window {
#include "../../runtime/src/gx/native_settings_host.inl"
}  // namespace host_window

#include "../../runtime/src/gx/native_settings_api.inl"

}  // namespace galaxy::gx

namespace galaxy::gx {

bool native_settings_overlay_visible() noexcept {
    return s_native_settings_visible.load(std::memory_order_relaxed);
}

// The game's own keys for the menu (a controller's, while it is open): handled at once, as the Windows window's
// message would.
void native_settings_post_key(std::uintptr_t virtual_key) noexcept {
    (void)handle_native_settings_key(static_cast<WPARAM>(virtual_key), false, true);
}

}  // namespace galaxy::gx

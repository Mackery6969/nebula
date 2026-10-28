// SPDX-License-Identifier: GPL-3.0-only
// Nebula's in-game settings menu, part four: the host API that reads and drives it (gx_d3d12.h, inside namespace
// galaxy::gx).
// host_window_win32.cpp and platform/sdl/host_window_sdl.cpp include it, the first in its place among the window's
// own code; neither builds alone.

NativeSettingsOverlaySnapshot native_settings_overlay_snapshot() noexcept {
    const unsigned tab =
        s_native_settings_tab.load(std::memory_order_relaxed) %
        kNativeSettingsTabCount;
    const unsigned count = native_settings_row_count(tab);
    unsigned row = s_native_settings_row.load(std::memory_order_relaxed);
    if (count != 0u) {
        row = std::min(row, count - 1u);
    } else {
        row = 0u;
    }
    NativeSettingsOverlaySnapshot snapshot{};
    snapshot.visible =
        s_native_settings_visible.load(std::memory_order_relaxed);
    snapshot.dirty =
        s_native_settings_dirty.load(std::memory_order_acquire);
    snapshot.save_error =
        s_native_settings_save_error.load(std::memory_order_acquire);
    snapshot.tab = tab;
    snapshot.row = row;
    snapshot.tab_count = kNativeSettingsTabCount;
    snapshot.row_count = count;
    {
        const std::lock_guard<std::recursive_mutex> lock(s_native_settings_mutex);
        snapshot.pending_changes = snapshot.visible ? pending_settings_changes() : 0u;
        snapshot.confirming_display = settings_display_confirming();
    }
    return snapshot;
}
NativeSettingsLayout compute_native_settings_layout(
    unsigned backbuffer_width,
    unsigned backbuffer_height,
    unsigned tab) noexcept {
    NativeSettingsLayout layout{};
    if (backbuffer_width < 427u || backbuffer_height < 240u) {
        return layout;
    }

    const int width = static_cast<int>(backbuffer_width);
    const int height = static_cast<int>(backbuffer_height);
    const bool compact =
        backbuffer_width < 854u || backbuffer_height < 480u;
    const int margin = compact
        ? 8
        : std::clamp(
              static_cast<int>(
                  std::min(backbuffer_width, backbuffer_height) / 24u),
              16,
              48);
    layout.supported = true;
    layout.panel_x = margin;
    layout.panel_y = margin;
    layout.panel_width = std::min(1040, width - margin * 2);
    layout.text_scale = compact
        ? 1
        : (backbuffer_width >= 2560u && backbuffer_height >= 1440u
               ? 4
               : (backbuffer_width >= 1280u ? 3 : 2));
    layout.rows_y = compact
        ? layout.panel_y + 46
        : layout.panel_y + 110 + (layout.text_scale - 2) * 8;
    layout.row_step = compact
        ? 14
        : std::max(32, layout.text_scale * 9 + 8);
    // Tall enough for the longest page (ten rows) above the footer.
    const int footer_block = compact ? 62 : 82;
    constexpr int kLongestPageRows = 10;
    const int needed_height = (layout.rows_y - layout.panel_y) +
        (kLongestPageRows - 1) * layout.row_step + layout.text_scale * 7 + 8 +
        footer_block;
    const int desired_height = compact
        ? 224
        : std::max(layout.text_scale >= 4 ? 480 : 420, needed_height);
    layout.panel_height = std::min(desired_height, height - margin * 2);
    layout.footer_y = layout.panel_y + layout.panel_height - footer_block;
    // A short window tightens the rows rather than dropping the menu.
    const int rows_room = layout.footer_y - 8 - layout.text_scale * 7 - layout.rows_y;
    if ((kLongestPageRows - 1) * layout.row_step > rows_room) {
        layout.row_step = std::max(layout.text_scale * 7 + 2,
                                   rows_room / (kLongestPageRows - 1));
    }

    const unsigned row_count =
        std::min(native_settings_row_count(tab), kNativeSettingsVisibleRows);
    const int final_row_bottom = row_count == 0u
        ? layout.rows_y
        : layout.rows_y +
              static_cast<int>(row_count - 1u) * layout.row_step +
              layout.text_scale * 7;
    if (layout.panel_width <= 0 || layout.panel_height <= 0 ||
        final_row_bottom + 8 > layout.footer_y) {
        return NativeSettingsLayout{};
    }
    return layout;
}

bool native_settings_handle_window_key(
    std::uintptr_t virtual_key,
    bool was_down) {
    return handle_native_settings_key(
        static_cast<WPARAM>(virtual_key),
        was_down);
}

bool native_settings_typing() noexcept {
    return s_settings_typing.load(std::memory_order_acquire);
}

bool native_settings_type(char32_t character) {
    const std::lock_guard<std::recursive_mutex> lock(s_native_settings_mutex);
    if (!s_settings_text) return false;
    if (s_settings_text_swallow != 0u) {
        // The space that started the typing comes right after its key.
        const bool fresh = GetTickCount64() < s_settings_text_swallow;
        s_settings_text_swallow = 0u;
        if (fresh && character == U' ') return true;
    }
    if (character >= 0x20u && character < 0x7Fu) (void)s_settings_text->type(character);
    return true;
}

NativeSettingsView native_settings_view() {
    s_settings_drawn_by_game.store(true, std::memory_order_release);
    return host_window::build_native_settings_view();
}

void native_settings_input(NativeSettingsInput input, int arg) {
    const std::lock_guard<std::recursive_mutex> lock(s_native_settings_mutex);
    const bool visible = s_native_settings_visible.load(std::memory_order_relaxed);
    if (visible && s_settings_text) {
        // The name being typed: a press (A, a click) or Apply keeps it, Close drops it, Revert deletes the letter at
        // the cursor; the rest waits until it is kept or dropped.
        switch (input) {
        case NativeSettingsInput::Press:
        case NativeSettingsInput::Apply: end_settings_text(true); break;
        case NativeSettingsInput::Close: end_settings_text(false); break;
        case NativeSettingsInput::Revert: s_settings_text->erase(); break;
        default: break;
        }
        return;
    }
    switch (input) {
    case NativeSettingsInput::Open:
        set_native_settings_visible(true);
        return;
    case NativeSettingsInput::Close:
        if (visible) (void)handle_native_settings_key(VK_ESCAPE, false, true);
        return;
    case NativeSettingsInput::Apply:
        if (visible) (void)handle_native_settings_key(VK_RETURN, false, true);
        return;
    case NativeSettingsInput::Revert:
        if (visible) (void)handle_native_settings_key(VK_BACK, false, true);
        return;
    default:
        break;
    }
    // The page and its rows wait while a question is up (keep the display? leave without applying?).
    if (!visible || s_settings_leave_asking || s_joycon_pairing || settings_display_confirming() ||
        s_mods_restart_ask.load(std::memory_order_acquire)) {
        return;
    }
    switch (input) {
    case NativeSettingsInput::SelectPage: {
        const int current = static_cast<int>(s_native_settings_tab.load(std::memory_order_relaxed) % kNativeSettingsTabCount);
        if (arg >= 0 && arg < static_cast<int>(kNativeSettingsTabCount) && arg != current) {
            cycle_native_settings_tab(arg - current);
        }
        return;
    }
    case NativeSettingsInput::SelectRow: {
        const unsigned tab = s_native_settings_tab.load(std::memory_order_relaxed) % kNativeSettingsTabCount;
        if (arg >= 0 && static_cast<unsigned>(arg) < native_settings_row_count(tab) &&
            !native_settings_heading(tab, static_cast<unsigned>(arg))) {
            end_controls_capture();
            if (static_cast<unsigned>(arg) != s_native_settings_row.load(std::memory_order_relaxed)) {
                s_native_settings_button_row.store(~0u, std::memory_order_relaxed);
            }
            s_native_settings_row.store(static_cast<unsigned>(arg), std::memory_order_relaxed);
            if (tab == kModsTab) {
                // The mod the buttons' Options and Remove act on.
                const std::vector<SettingsRow> rows = mods_page(s_settings_draft);
                if (static_cast<unsigned>(arg) < rows.size() && rows[static_cast<unsigned>(arg)].key.rfind("mod:", 0) == 0) {
                    const std::lock_guard<std::mutex> mods_lock(s_mods_mutex);
                    s_mod_focus = rows[static_cast<unsigned>(arg)].key.substr(4u);
                }
            }
        }
        return;
    }
    case NativeSettingsInput::Change:
        if (arg == -2 || arg == 2) {
            step_native_setting_round(arg / 2);
        } else if (arg != 0) {
            adjust_native_setting(arg < 0 ? -1 : 1);
        }
        return;
    case NativeSettingsInput::Press:
        if (selected_settings_text()) {
            start_settings_text(false);
        } else if (arg >= 0 && static_cast<std::size_t>(arg) < selected_settings_buttons()) {
            press_settings_button(static_cast<unsigned>(arg));
        }
        return;
    case NativeSettingsInput::SetValue:
        if (arg >= 0) set_slider(static_cast<unsigned>(arg));
        return;
    case NativeSettingsInput::MoveMod: {
        // The selected mod dropped into a list (arg >> 16: 1 off, 2 on) at a place (its low bits; 0 the top).
        const unsigned tab = s_native_settings_tab.load(std::memory_order_relaxed) % kNativeSettingsTabCount;
        const std::vector<SettingsRow> rows = tab == kModsTab ? mods_page(s_settings_draft) : std::vector<SettingsRow>{};
        const unsigned row = s_native_settings_row.load(std::memory_order_relaxed);
        if (arg < 0 || row >= rows.size() || rows[row].list == 0u || rows[row].heading) return;
        const std::string id = rows[row].key.substr(4u);
        {
            const std::lock_guard<std::mutex> mods_lock(s_mods_mutex);
            place_mod_locked(id, (arg >> 16) == 2, static_cast<std::size_t>(arg & 0xFFFF));
            s_mod_held.clear();
            s_mod_focus = id;
        }
        select_mods_row(s_settings_draft, "mod:" + id);
        return;
    }
    default:
        return;
    }
}

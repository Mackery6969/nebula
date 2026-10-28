// SPDX-License-Identifier: GPL-3.0-only
// Nebula's in-game settings menu, part three: its saves and the view the game draws (inside namespace host_window).
// host_window_win32.cpp and platform/sdl/host_window_sdl.cpp include it, the first in its place among the window's
// own code; neither builds alone.

// DXR 1.1 with shader model 6.5 on an adapter with dedicated memory (the RAY TRACING row).
std::atomic<bool> ray_tracing_offered{false};
// Queues a save of the settings files when something changed; never waits on the disk.
bool flush_native_settings_if_dirty() {
    if (!s_native_settings_dirty.load(std::memory_order_acquire)) {
        return true;
    }
    static std::once_flag started;
    std::call_once(started, [] { std::thread(settings_save_worker).detach(); });
    {
        const std::lock_guard<std::mutex> lock(s_settings_save_mutex);
        s_settings_save_requested = true;
    }
    s_settings_save_changed.notify_all();
    return true;  // queued: the worker reports a failed write and keeps the settings dirty
}

unsigned upscaler_render_scale(const RenderConfig& render, unsigned internal) {
    return gx::upscaler_render_scale(render, internal);
}

// Exit paths: waits for a queued or running save (at most 5 s), then writes what is still unsaved here.
void save_native_settings_now() {
    {
        std::unique_lock<std::mutex> lock(s_settings_save_mutex);
        s_settings_save_changed.wait_for(lock, std::chrono::seconds(5), [] {
            return !s_settings_save_requested && !s_settings_save_running;
        });
    }
    (void)write_dirty_settings_files();
}

// Nebula Settings can save config.ini while the game runs. The game takes the file's settings as the menu's
// apply does, so the next save from the game writes them back rather than over them. Checked twice a second,
// and not while the menu is open: its draft and display confirmation belong to the settings it opened with, and
// a change made meanwhile lands once it closes.
void poll_render_config_file() {
    static std::uint64_t s_next_check_ms = 0u;
    const std::uint64_t now = GetTickCount64();
    if (now < s_next_check_ms) {
        return;
    }
    s_next_check_ms = now + 500u;
    // A save of the game's own still in flight would read back as a change made outside it.
    if (settings_save_pending()) {
        return;
    }
    const std::lock_guard<std::recursive_mutex> lock(s_native_settings_mutex);
    if (s_native_settings_visible.load(std::memory_order_relaxed)) {
        return;
    }
    const RenderConfig before = get_render_config();
    const std::string gpu_before = next_start_gpu();
    if (!reload_render_config_if_changed()) {
        return;
    }
    RenderConfig after = get_render_config();
    if (after.internal_follow_window) {
        // The window decides the scale, as when the menu turns Match Output on.
        after.efb_scale = before.efb_scale;
        set_render_config(after);
        host_window::follow_window_rearm.store(true, std::memory_order_release);
    }
    std::cerr << "[settings] config.ini reloaded\n";
    if (const std::string gpu = next_start_gpu(); gpu != gpu_before) {
        std::cerr << "[settings] gpu=" << gpu << " applies at the next start\n";
    }
}

// A GPU chosen in Nebula Settings during the session that is not the one the game runs on.
bool gpu_change_pending(const RenderConfig& render, const RenderFeatureCaps& caps) {
    const std::string next = next_start_gpu();
    return next != render.gpu.data() &&
           next != gpu_identity(caps.adapter_vendor_id, caps.adapter_device_id, caps.adapter_subsys_id);
}

// The menu as it stands: its pages, the open page's rows (label, value, a heading or not, whether it changes), the
// selection and what the line under the page says, in the order the overlay always chose it. Takes the menu's lock.
NativeSettingsView build_native_settings_view() {
    NativeSettingsView view;
    view.pages.assign(std::begin(kNativeSettingsTabNames), std::end(kNativeSettingsTabNames));
    view.controls_page = kControlsTab;
    // The hints show the buttons the icons do: a chosen style whatever is in hand, else what the player last used.
    const ButtonIcons icons = get_runtime_settings().button_icons;
    if (icons == ButtonIcons::Xbox || icons == ButtonIcons::Nintendo || icons == ButtonIcons::PlayStation) {
        view.input = icons == ButtonIcons::Xbox ? 1u : icons == ButtonIcons::Nintendo ? 2u : 3u;
    } else if (const auto device = galaxy::host::last_input_device()) {
        switch (device->scheme) {
        case galaxy::input::ControlScheme::Keyboard: break;
        case galaxy::input::ControlScheme::SwitchPro:
        case galaxy::input::ControlScheme::SwitchPro2: view.input = 2u; break;
        case galaxy::input::ControlScheme::PlayStation: view.input = 3u; break;
        default: view.input = 1u; break;
        }
    }
    view.open = s_native_settings_visible.load(std::memory_order_relaxed);
    const unsigned tab = s_native_settings_tab.load(std::memory_order_relaxed) % kNativeSettingsTabCount;
    view.page = tab;
    if (!view.open) {
        return view;
    }
    const std::lock_guard<std::recursive_mutex> settings_lock(s_native_settings_mutex);
    expire_display_confirmation();
    expire_controls_capture();
    if (tab == kControlsTab) {
        refresh_controls_devices(false);
    }
    const RenderConfig row_render = s_settings_draft;
    const RuntimeSettings row_runtime = s_runtime_draft;
    const RenderFeatureCaps row_caps = get_render_feature_caps();
    const unsigned row_count = native_settings_row_count(tab);
    view.selected = std::min(s_native_settings_row.load(std::memory_order_relaxed), row_count == 0u ? 0u : row_count - 1u);
    const bool page_tab = settings_page_tab(tab);
    const std::vector<SettingsRow> page =
        page_tab ? settings_page(tab, row_render, row_runtime, row_caps) : std::vector<SettingsRow>{};
    view.rows.reserve(row_count);
    for (unsigned row = 0; row < row_count; ++row) {
        NativeSettingsViewRow shown;
        shown.changes = true;
        if (page_tab && row < page.size()) {
            shown.label = page[row].label;
            shown.value = page[row].value;
            shown.heading = page[row].heading;
            shown.changes = static_cast<bool>(page[row].change);
            shown.label_name = (page[row].names & 1u) != 0u;
            shown.name = (page[row].names & 2u) != 0u;
            shown.mod_list = page[row].list;
            shown.held = page[row].held;
            shown.buttons = !page[row].buttons.empty();
            shown.text = page[row].text;
            if (row == view.selected) view.help = page[row].help;
        } else {
            shown.label = native_settings_row_text(tab, row, row_render, row_runtime, row_caps);
            if (const std::size_t colon = shown.label.find(": "); colon != std::string::npos) {
                shown.value = shown.label.substr(colon + 2u);
                shown.label.resize(colon);
            }
        }
        shown.name = shown.name || (tab == 0u && shown.label == "MONITOR") ||
                     (tab == kSystemTab && shown.label == "GRAPHICS CARD") ||
                     (tab == kGeneralTab && (shown.label == "GALAXY" || shown.label == "STAR"));
        if (tab == kControlsTab) {
            // The controller being set up by its own name (the overlay's row has it in capitals), and the bindings as
            // lists of inputs.
            const ControlsRowInfo info = controls_row(row);
            if (info.kind == ControlsRow::Device) {
                const galaxy::input::DeviceEntry& entry =
                    s_controls_devices.entries[galaxy::input::resolve_device(s_controls_devices, s_controls_device)];
                shown.value = entry.label.data();
                shown.name = true;
                shown.active = entry.active;
            }
            const bool capturing = controls_capturing() && s_capture_target == static_cast<std::size_t>(info.target);
            if (info.kind == ControlsRow::Binding && !capturing) {
                shown.value = galaxy::input::format_target_bindings(s_controls_scheme,
                                                                    s_controls_draft.table(s_controls_scheme)[info.target]);
            }
            shown.inputs = info.kind == ControlsRow::Binding;
            if (info.kind == ControlsRow::Reset) {
                shown.buttons = true;
                shown.value = "RESET";
            } else if (info.kind == ControlsRow::PairJoyCon) {
                shown.buttons = true;
                shown.value = "PAIR";
            }
        }
        view.rows.push_back(std::move(shown));
    }
    view.selected_button = view.selected < view.rows.size() && view.rows[view.selected].buttons ? selected_settings_button() : 0u;
    view.editing = s_settings_text.has_value();
    for (unsigned row = 0; row < view.rows.size(); ++row) {
        const SliderPlace place = settings_slider(tab, row, page);
        view.rows[row].slider_steps = place.steps;
        view.rows[row].slider_at = place.at;
    }

    const std::uint64_t now_ms = GetTickCount64();
    if (s_joycon_pairing) {
        // The halves; a pair made since the screen opened shows a moment, then the screen closes.
        const galaxy::host::JoyConStatus joycons = joycon_status_now();
        if (joycons.paired != s_joycon_paired_at_open && s_joycon_paired_ms == 0u) {
            s_joycon_paired_ms = now_ms;
            std::cerr << "[settings] joy-con paired\n";
        }
        view.status = NativeSettingsStatus::JoyCon;
        view.status_number = (joycons.lefts != 0u ? kJoyConLeftBit : 0) | (joycons.rights != 0u ? kJoyConRightBit : 0) |
                             (joycons.left_held ? kJoyConLeftHeldBit : 0) | (joycons.right_held ? kJoyConRightHeldBit : 0) |
                             (s_joycon_paired_ms != 0u ? kJoyConPairedBit : 0) |
                             (joycons.left_presses & 15) << kJoyConLeftPressesShift |
                             (joycons.right_presses & 15) << kJoyConRightPressesShift;
        if (s_joycon_paired_ms != 0u && now_ms - s_joycon_paired_ms >= kJoyConPairedShowMs) {
            close_joycon_pairing();
        }
    } else if (s_settings_leave_asking) {
        view.status = NativeSettingsStatus::Leave;
        view.status_number = static_cast<int>(pending_settings_changes());
    } else if (s_mods_restart_ask.load(std::memory_order_acquire)) {
        view.status = NativeSettingsStatus::ModsRestartAsk;
    } else if (controls_capturing()) {
        view.status = s_controls_scheme == galaxy::input::ControlScheme::Keyboard ? NativeSettingsStatus::CaptureKey
                                                                                   : NativeSettingsStatus::CapturePad;
    } else if (s_controls_message_until_ms > now_ms) {
        view.status = NativeSettingsStatus::Message;
        view.status_text = s_controls_message;
    } else if (const NativeSettingsStatus dlss = dlss_status(now_ms); dlss != NativeSettingsStatus::Help) {
        view.status = dlss;
        view.status_number = kDlssDownloadMegabytes;
    } else if (settings_display_confirming()) {
        const std::uint64_t left_ms = s_display_confirm_deadline_ms > now_ms ? s_display_confirm_deadline_ms - now_ms : 0u;
        view.status = NativeSettingsStatus::KeepDisplay;
        view.status_number = static_cast<int>((left_ms + 999u) / 1000u);
    } else if (s_native_settings_save_error.load(std::memory_order_acquire)) {
        view.status = NativeSettingsStatus::SaveFailed;
    } else if (const unsigned pending = pending_settings_changes(); pending != 0u) {
        view.status = NativeSettingsStatus::Pending;
        view.status_number = static_cast<int>(pending);
    } else if (s_settings_applied_ms != 0u && now_ms - s_settings_applied_ms < 2500u) {
        view.status = NativeSettingsStatus::Applied;
    } else if (GameLanguage running{}; tab == kGeneralTab && running_game_language(running) && running != row_runtime.language) {
        view.status = NativeSettingsStatus::LanguageRestart;
    } else if (tab == kModsTab && mods_restart_pending()) {
        view.status = NativeSettingsStatus::ModsRestart;
    } else if (tab == kSystemTab && gpu_change_pending(row_render, row_caps)) {
        view.status = NativeSettingsStatus::GpuRestart;
    }
    return view;
}

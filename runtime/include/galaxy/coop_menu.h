// The online co-op main menu (docs/COOP_PLAN.md, "Main menu"): its pages, rows and choices, kept by the host for
// every platform. The game draws it with the settings menu's screen (NebulaSettingsMenu) and sends its input back;
// what the player chose (play, quit, the settings) waits until the game takes it.
//
// The main page: Host, Join, Options, Quit. Host opens the Host page: the save to play (left and right through the
// saves, then New save), Erase, the room's settings (room(): the coop_* runtime settings) and Host, which plays; B
// or Back returns to the main page.
#pragma once

#include "galaxy/runtime_settings.h"

#include <string>
#include <vector>

namespace galaxy::coop {

class SaveStore;

struct MenuRow {
    std::string label;
    std::string value;  // a button row's words, '|' between them, '-' before an unavailable one
    std::string help;   // the line under the page while the row is selected
    bool buttons = false;
    bool changes = false;  // left and right change its value
    bool name = false;     // its value is a name (a save's), shown as is
};

struct MenuView {
    bool open = false;
    unsigned page = 0u;
    std::vector<std::string> pages;
    std::vector<MenuRow> rows;  // the page's
    unsigned selected = 0u;
    unsigned selected_button = 0u;
};

// What the player chose, for the game to act on (MainMenu::take_command).
enum class MenuCommand { None, Play, Quit, Settings };

class MainMenu {
public:
    // `settings`: this host has Nebula's in-game settings menu to open (Options). `saves`: the co-op saves the Host
    // page picks from (none: Host plays without one).
    explicit MainMenu(bool settings, SaveStore* saves = nullptr);
    void set_saves(SaveStore* saves) noexcept { saves_ = saves; }

    // The room's settings the Host page shows and changes; the host keeps them as the runtime settings.
    void set_room(const RuntimeCoopSettings& room) { room_ = room; }
    [[nodiscard]] const RuntimeCoopSettings& room() const noexcept { return room_; }

    void open();
    void close();
    [[nodiscard]] bool is_open() const noexcept { return open_; }
    [[nodiscard]] MenuView view() const;

    void select_page(unsigned page);
    void select_row(unsigned row);
    // Up or down `delta` rows, past the ends to the other end.
    void move(int delta);
    // Left or right: the selected row's next or previous button, or its next or previous value.
    void change(int delta);
    // The selected row's `button` pressed (the pointer names it; A presses the selected one).
    void press(unsigned button);
    // B: the Host page back to the main page.
    void back();

    // The last choice, once: None after it was taken.
    [[nodiscard]] MenuCommand take_command() noexcept;
    // After Play: the save Host plays, its id ("" for a new one, which the host makes).
    [[nodiscard]] const std::string& chosen_save() const noexcept { return chosen_save_; }

private:
    enum class Screen { Main, Host };

    [[nodiscard]] std::vector<MenuRow> rows() const;
    [[nodiscard]] unsigned row_count() const;
    [[nodiscard]] unsigned button_count(unsigned row) const;
    void show(Screen screen);
    void press_host(unsigned row);

    bool settings_ = false;
    SaveStore* saves_ = nullptr;
    RuntimeCoopSettings room_{};
    bool open_ = false;
    Screen screen_ = Screen::Main;
    unsigned selected_ = 0u;
    unsigned selected_button_ = 0u;
    MenuCommand command_ = MenuCommand::None;
    // The Host page: the saves (their ids) as it opened, the one picked (their count: a new save), and Erase asked
    // once.
    std::vector<std::string> save_ids_;
    std::vector<std::string> save_names_;
    unsigned save_ = 0u;
    bool erase_armed_ = false;
    std::string chosen_save_;
};

}  // namespace galaxy::coop

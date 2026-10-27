// The online co-op main menu (docs/COOP_PLAN.md, "Main menu"): its pages, rows and choices, kept by the host for
// every platform. The game draws it with the settings menu's screen (NebulaSettingsMenu) and sends its input back;
// what the player chose (play, quit, the settings) waits until the game takes it.
#pragma once

#include <string>
#include <vector>

namespace galaxy::coop {

struct MenuRow {
    std::string label;
    std::string value;  // a button row's words, '|' between them, '-' before an unavailable one
    std::string help;   // the line under the page while the row is selected
    bool buttons = false;
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
    // `settings`: this host has Nebula's in-game settings menu to open (Options).
    explicit MainMenu(bool settings);

    void open();
    void close();
    [[nodiscard]] bool is_open() const noexcept { return open_; }
    [[nodiscard]] MenuView view() const;

    void select_page(unsigned page);
    void select_row(unsigned row);
    // Up or down `delta` rows, past the ends to the other end.
    void move(int delta);
    // Left or right: the selected row's next or previous button.
    void change(int delta);
    // The selected row's `button` pressed (the pointer names it; A presses the selected one).
    void press(unsigned button);

    // The last choice, once: None after it was taken.
    [[nodiscard]] MenuCommand take_command() noexcept;

private:
    [[nodiscard]] std::vector<MenuRow> rows() const;
    [[nodiscard]] unsigned button_count(unsigned row) const;

    bool settings_ = false;
    bool open_ = false;
    unsigned page_ = 0u;
    unsigned selected_ = 0u;
    unsigned selected_button_ = 0u;
    MenuCommand command_ = MenuCommand::None;
};

}  // namespace galaxy::coop

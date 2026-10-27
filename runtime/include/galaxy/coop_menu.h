// The online co-op main menu (docs/COOP_PLAN.md, "Main menu"): its pages, rows and choices, kept by the host for
// every platform. The game draws it with the settings menu's screen (NebulaSettingsMenu) and sends its input back;
// what the player chose (play, quit, the settings) waits until the game takes it.
//
// The main page: Host, Join, Player, Options, Quit. Host opens the Host page: the save to play (left and right through
// the saves, then New save), Rename and Erase, the room's port and password, its settings (room(): the coop_* runtime
// settings), the mods it plays with (a page of their own) and Host, which plays. Join opens the Join page (the
// address and password; joining itself comes with online play), Player the Player page (the name and character). B
// or Back returns to the main page.
//
// Text rows (a name, a port, an address, a password) are edited in place: pressed, they take the keyboard's typing
// (type()) or, letter by letter, a controller's: up and down pick the letter at the cursor, left and right move it,
// Minus (Tab's reset) deletes, A keeps the text and B drops it.
#pragma once

#include "galaxy/runtime_settings.h"

#include <functional>
#include <string>
#include <vector>

namespace galaxy::coop {

class SaveStore;

struct MenuRow {
    std::string label;
    std::string value;  // a button row's words, '|' between them, '-' before an unavailable one
    std::string help;   // the line under the page while the row is selected
    bool buttons = false;
    bool changes = false;     // left and right change its value
    bool name = false;        // its value is a name (a save's, a player's, typed text), shown as is
    bool text = false;        // a text row: pressing it edits its value
    bool label_name = false;  // its label is a name (a mod's), shown as is
};

struct MenuView {
    bool open = false;
    unsigned page = 0u;
    std::vector<std::string> pages;
    std::vector<MenuRow> rows;  // the page's
    unsigned selected = 0u;
    unsigned selected_button = 0u;
    bool back = false;     // a page under the main one: B leaves it, Tab resets its rows
    bool editing = false;  // the selected row's text is being edited
};

// What the player chose, for the game to act on (MainMenu::take_command).
enum class MenuCommand { None, Play, Quit, Settings };

// A mod of the mods folder as the Host page picks it for the room: on or off, the ones on first, the top one winning.
struct RoomMod {
    std::string id;
    bool on = false;

    friend bool operator==(const RoomMod&, const RoomMod&) = default;
};

// The keys a text row takes besides the characters typed (type()).
enum TextKey : char32_t {
    kTextBackspace = 0x08,
    kTextEnter = 0x0D,
    kTextEscape = 0x1B,
    kTextDelete = 0x7F,
    kTextLeft = 0xF700,
    kTextRight,
    kTextUp,
    kTextDown,
    kTextHome,
    kTextEnd,
};

// The longest player name the Player page takes (nametags stay short).
inline constexpr std::size_t kPlayerNameLimit = 20u;

class MainMenu {
public:
    // `settings`: this host has Nebula's in-game settings menu to open (Options). `saves`: the co-op saves the Host
    // page picks from (none: Host plays without one).
    explicit MainMenu(bool settings, SaveStore* saves = nullptr);
    void set_saves(SaveStore* saves) noexcept { saves_ = saves; }
    // The mods folder's mods, the ones on (as last hosted) first in their order, read again as the Host page opens.
    void set_mods_source(std::function<std::vector<RoomMod>()> source) { mods_source_ = std::move(source); }

    // The settings the menu shows and changes (the room's on the Host page, the player's, the last address); the host
    // keeps them as the runtime settings.
    void set_room(const RuntimeCoopSettings& room) { room_ = room; }
    [[nodiscard]] const RuntimeCoopSettings& room() const noexcept { return room_; }
    // The room's mods as the Host page has them, and whether they differ from what it read.
    [[nodiscard]] const std::vector<RoomMod>& room_mods() const noexcept { return mods_; }
    [[nodiscard]] bool mods_changed() const noexcept { return mods_ != mods_read_; }

    void open();
    void close();
    [[nodiscard]] bool is_open() const noexcept { return open_; }
    [[nodiscard]] MenuView view() const;

    void select_page(unsigned page);
    void select_row(unsigned row);
    // Up or down `delta` rows, past the ends to the other end; while a text is edited, the letter at its cursor.
    void move(int delta);
    // Left or right (-1 or 1): the selected row's next or previous button, or its next or previous value; -2 or 2
    // (a click on the row) the same, going round past its ends. While a text is edited, its cursor.
    void change(int delta);
    // The selected row's `button` pressed (the pointer names it; A presses the selected one). A text row starts or
    // ends its editing.
    void press(unsigned button);
    // B: a page under the main one back to the main page, the mods back to the Host page; a text edited is dropped.
    void back();
    // Tab (or the controller's Back): the selected setting back to its default; while a text is edited, the letter
    // at the cursor deleted.
    void reset();
    // A key typed (a printable character or a TextKey) into the text being edited; false when none is.
    bool type(char32_t key);
    [[nodiscard]] bool editing() const noexcept { return edit_.field != Field::None; }

    // A line under the page in place of the selected row's help, until the player moves on.
    void set_message(std::string message) { message_ = std::move(message); }

    // The last choice, once: None after it was taken.
    [[nodiscard]] MenuCommand take_command() noexcept;
    // After Play: the save Host plays, its id ("" for a new one, which the host makes).
    [[nodiscard]] const std::string& chosen_save() const noexcept { return chosen_save_; }
    // Play `id` at once, as Host on it would (the start Host restarted Nebula for, with the room's mods).
    void play(std::string id);

private:
    enum class Screen { Main, Host, Mods, Join, Player };
    enum class Field { None, SaveName, Port, Password, Address, PlayerName };
    struct Edit {
        Field field = Field::None;
        unsigned row = 0u;
        std::string text;
        std::size_t cursor = 0u;
    };

    [[nodiscard]] std::vector<MenuRow> rows() const;
    [[nodiscard]] unsigned row_count() const;
    [[nodiscard]] unsigned button_count(unsigned row) const;
    // The row's value: its text, or the text being edited with its cursor.
    [[nodiscard]] std::string text_value(Field field, unsigned row, std::string value) const;
    void show(Screen screen, unsigned row = 0u);
    void press_main(unsigned row);
    void press_host(unsigned row, unsigned button);
    void press_mods(unsigned row);
    void start_edit(Field field, unsigned row);
    void finish_edit();
    void step_letter(int delta);
    void read_mods();
    void toggle_mod(unsigned index);

    bool settings_ = false;
    SaveStore* saves_ = nullptr;
    std::function<std::vector<RoomMod>()> mods_source_;
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
    // The room's mods as read, and as picked.
    std::vector<RoomMod> mods_read_;
    std::vector<RoomMod> mods_;
    Edit edit_;
    std::string message_;
};

}  // namespace galaxy::coop

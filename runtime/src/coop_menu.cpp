#include "galaxy/coop_menu.h"

#include <algorithm>
#include <iterator>
#include <utility>

namespace galaxy::coop {

namespace {

// The pages, in order, and the PLAY page's rows.
constexpr const char* kPages[] = {"PLAY"};
enum PlayRow : unsigned { RowHost, RowJoin, RowOptions, RowQuit, RowCount };

}  // namespace

MainMenu::MainMenu(bool settings) : settings_(settings) {}

void MainMenu::open() {
    if (open_) return;
    open_ = true;
    page_ = 0u;
    selected_ = RowHost;
    selected_button_ = 0u;
    command_ = MenuCommand::None;
}

void MainMenu::close() {
    open_ = false;
}

std::vector<MenuRow> MainMenu::rows() const {
    std::vector<MenuRow> rows(RowCount);
    rows[RowHost] = {"HOST", "PLAY", "PLAYS YOUR SAVE. HOSTING FOR OTHER PLAYERS COMES WITH ONLINE PLAY", true};
    rows[RowJoin] = {"JOIN", "-JOIN", "JOINING ANOTHER PLAYER'S GAME COMES WITH ONLINE PLAY", true};
    rows[RowOptions] = settings_
        ? MenuRow{"OPTIONS", "OPEN", "NEBULA'S SETTINGS: DISPLAY, GRAPHICS, CONTROLS, AUDIO", true}
        : MenuRow{"OPTIONS", "-OPEN", "NEBULA'S IN-GAME SETTINGS ARE WINDOWS-ONLY FOR NOW", true};
    rows[RowQuit] = {"QUIT", "QUIT TO DESKTOP", "CLOSES NEBULA", true};
    return rows;
}

unsigned MainMenu::button_count(unsigned row) const {
    const std::vector<MenuRow> all = rows();
    if (row >= all.size() || !all[row].buttons) return 0u;
    return static_cast<unsigned>(std::count(all[row].value.begin(), all[row].value.end(), '|')) + 1u;
}

MenuView MainMenu::view() const {
    MenuView view;
    view.open = open_;
    view.page = page_;
    view.pages.assign(std::begin(kPages), std::end(kPages));
    view.rows = rows();
    view.selected = selected_;
    view.selected_button = selected_button_;
    return view;
}

void MainMenu::select_page(unsigned page) {
    if (page >= std::size(kPages) || page == page_) return;
    page_ = page;
    selected_ = 0u;
    selected_button_ = 0u;
}

void MainMenu::select_row(unsigned row) {
    if (row >= RowCount || row == selected_) return;
    selected_ = row;
    selected_button_ = 0u;
}

void MainMenu::move(int delta) {
    if (delta == 0) return;
    const int count = static_cast<int>(RowCount);
    const int next = ((static_cast<int>(selected_) + delta) % count + count) % count;
    select_row(static_cast<unsigned>(next));
}

void MainMenu::change(int delta) {
    const unsigned count = button_count(selected_);
    if (count < 2u || delta == 0) return;
    const int next = std::clamp(static_cast<int>(selected_button_) + (delta < 0 ? -1 : 1), 0, static_cast<int>(count) - 1);
    selected_button_ = static_cast<unsigned>(next);
}

void MainMenu::press(unsigned button) {
    if (!open_ || button >= button_count(selected_)) return;
    // An unavailable button ('-' before its word) does nothing.
    const std::string value = rows()[selected_].value;
    std::size_t start = 0u;
    for (unsigned i = 0u; i < button; ++i) start = value.find('|', start) + 1u;
    if (value[start] == '-') return;
    switch (selected_) {
    case RowHost: command_ = MenuCommand::Play; break;
    case RowOptions: command_ = MenuCommand::Settings; break;
    case RowQuit: command_ = MenuCommand::Quit; break;
    default: break;
    }
}

MenuCommand MainMenu::take_command() noexcept {
    return std::exchange(command_, MenuCommand::None);
}

}  // namespace galaxy::coop

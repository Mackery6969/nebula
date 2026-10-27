#include "galaxy/coop_menu.h"

#include "galaxy/coop_saves.h"

#include <algorithm>
#include <iterator>
#include <utility>

namespace galaxy::coop {

namespace {

enum MainRow : unsigned { RowHost, RowJoin, RowOptions, RowQuit, MainRowCount };
enum HostRow : unsigned {
    RowSave,
    RowErase,
    RowPlayers,
    RowInteraction,
    RowKnockback,
    RowPvp,
    RowCheats,
    RowStart,
    RowBack,
    HostRowCount
};

constexpr unsigned kKnockbacks[] = {kCoopKnockbackWeak, kCoopKnockbackNormal, kCoopKnockbackTooMuch};

// The choice nearest `strength` (an edited file's in between rounds to one).
unsigned knockback_index(unsigned strength) {
    unsigned best = 0u;
    for (unsigned i = 1u; i < std::size(kKnockbacks); ++i) {
        const auto distance = [strength](unsigned value) {
            return value > strength ? value - strength : strength - value;
        };
        if (distance(kKnockbacks[i]) < distance(kKnockbacks[best])) best = i;
    }
    return best;
}

const char* interaction_word(CoopPlayerInteraction interaction) {
    switch (interaction) {
    case CoopPlayerInteraction::NonSolid: return "NON-SOLID";
    case CoopPlayerInteraction::Solid: return "SOLID";
    case CoopPlayerInteraction::FriendlyFire: return "FRIENDLY FIRE";
    }
    return "SOLID";
}

// `value` stepped by `delta` within [0, count), stopping at the ends.
unsigned step(unsigned value, int delta, unsigned count) {
    return static_cast<unsigned>(
        std::clamp(static_cast<int>(value) + (delta < 0 ? -1 : 1), 0, static_cast<int>(count) - 1));
}

}  // namespace

MainMenu::MainMenu(bool settings, SaveStore* saves) : settings_(settings), saves_(saves) {}

void MainMenu::open() {
    if (open_) return;
    open_ = true;
    command_ = MenuCommand::None;
    show(Screen::Main);
}

void MainMenu::close() {
    open_ = false;
}

void MainMenu::show(Screen screen) {
    screen_ = screen;
    selected_ = 0u;
    selected_button_ = 0u;
    erase_armed_ = false;
    if (screen != Screen::Host) return;
    // The saves as the page opens, the last played first: it is the one picked.
    save_ids_.clear();
    save_names_.clear();
    if (saves_ != nullptr) {
        for (const SaveInfo& save : saves_->list()) {
            save_ids_.push_back(save.id);
            save_names_.push_back(save.name);
        }
    }
    save_ = 0u;
}

unsigned MainMenu::row_count() const {
    return screen_ == Screen::Main ? unsigned{MainRowCount} : unsigned{HostRowCount};
}

std::vector<MenuRow> MainMenu::rows() const {
    if (screen_ == Screen::Main) {
        std::vector<MenuRow> rows(MainRowCount);
        rows[RowHost] = {"HOST", "OPEN", "PICK A SAVE AND THE ROOM'S SETTINGS, THEN PLAY", true};
        rows[RowJoin] = {"JOIN", "-JOIN", "JOINING ANOTHER PLAYER'S GAME COMES WITH ONLINE PLAY", true};
        rows[RowOptions] = settings_
            ? MenuRow{"OPTIONS", "OPEN", "NEBULA'S SETTINGS: DISPLAY, GRAPHICS, CONTROLS, AUDIO", true}
            : MenuRow{"OPTIONS", "-OPEN", "NEBULA'S IN-GAME SETTINGS ARE WINDOWS-ONLY FOR NOW", true};
        rows[RowQuit] = {"QUIT", "QUIT TO DESKTOP", "CLOSES NEBULA", true};
        return rows;
    }

    const RuntimeCoopSettings& coop = room_;
    const bool fresh = save_ >= save_ids_.size();
    std::vector<MenuRow> rows(HostRowCount);
    rows[RowSave] = {"SAVE", fresh ? "NEW SAVE" : save_names_[save_],
                     "LEFT AND RIGHT PICK A SAVE, OR A NEW ONE. YOUR SAVES ARE YOURS: PLAYERS WHO JOIN PLAY IN THEM",
                     false, true, !fresh};
    rows[RowErase] = {"ERASE", fresh ? "-ERASE" : erase_armed_ ? "PRESS AGAIN TO ERASE" : "ERASE",
                      "ERASES THE SAVE AND ITS ACHIEVEMENTS FOR GOOD", true};
    rows[RowPlayers] = {"MAX PLAYERS", std::to_string(coop.max_players),
                        "HOW MANY PLAYERS THE ROOM HOLDS, YOU INCLUDED", false, true};
    rows[RowInteraction] = {"PLAYER INTERACTION", interaction_word(coop.player_interaction),
                            "NON-SOLID: PLAYERS PASS THROUGH. SOLID: THEY BUMP AND KNOCK BACK. FRIENDLY FIRE: ATTACKS "
                            "HURT",
                            false, true};
    static const char* const knockbacks[] = {"WEAK", "NORMAL", "TOO MUCH"};
    rows[RowKnockback] = {"KNOCKBACK STRENGTH", knockbacks[knockback_index(coop.knockback_strength)],
                          "HOW FAR A PLAYER'S ATTACK KNOCKS ANOTHER", false, true};
    rows[RowPvp] = {"PVP MODE", coop.pvp_mode == CoopPvpMode::Revamped ? "REVAMPED" : "CLASSIC",
                    "FRIENDLY FIRE'S RULES: CLASSIC, OR REVAMPED (WHICH ATTACKS BEAT WHICH)", false, true};
    rows[RowCheats] = {"CHEATS", coop.cheats ? "ON" : "OFF",
                       "ON: EVERYONE IN THE ROOM MAY USE THE ONLINE TAB'S CHEATS", false, true};
    rows[RowStart] = {"HOST", "HOST", "PLAYS THE SAVE. OTHER PLAYERS JOINING COMES WITH ONLINE PLAY", true};
    rows[RowBack] = {"BACK", "BACK", "BACK TO THE MAIN MENU", true};
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
    view.page = 0u;
    view.pages = {screen_ == Screen::Main ? "PLAY" : "HOST"};
    view.rows = rows();
    view.selected = selected_;
    view.selected_button = selected_button_;
    return view;
}

void MainMenu::select_page(unsigned) {
    // One page a screen: Host and Back move between them.
}

void MainMenu::select_row(unsigned row) {
    if (row >= row_count() || row == selected_) return;
    selected_ = row;
    selected_button_ = 0u;
    erase_armed_ = false;
}

void MainMenu::move(int delta) {
    if (delta == 0) return;
    const int count = static_cast<int>(row_count());
    const int next = ((static_cast<int>(selected_) + delta) % count + count) % count;
    select_row(static_cast<unsigned>(next));
}

void MainMenu::change(int delta) {
    if (delta == 0) return;
    if (screen_ == Screen::Host) {
        RuntimeCoopSettings& coop = room_;
        switch (selected_) {
        case RowSave:
            save_ = step(save_, delta, static_cast<unsigned>(save_ids_.size()) + 1u);
            erase_armed_ = false;
            return;
        case RowPlayers:
            coop.max_players =
                std::clamp(delta < 0 ? coop.max_players - 1u : coop.max_players + 1u, 2u, kCoopMaxPlayersLimit);
            break;
        case RowInteraction:
            coop.player_interaction = static_cast<CoopPlayerInteraction>(
                step(static_cast<unsigned>(coop.player_interaction), delta, 3u));
            break;
        case RowKnockback:
            coop.knockback_strength = kKnockbacks[step(knockback_index(coop.knockback_strength), delta, 3u)];
            break;
        case RowPvp:
            coop.pvp_mode = static_cast<CoopPvpMode>(step(static_cast<unsigned>(coop.pvp_mode), delta, 2u));
            break;
        case RowCheats: coop.cheats = delta > 0; break;
        default: break;
        }
    }
    const unsigned count = button_count(selected_);
    if (count < 2u) return;
    selected_button_ = step(selected_button_, delta, count);
}

void MainMenu::press(unsigned button) {
    if (!open_ || button >= button_count(selected_)) return;
    // An unavailable button ('-' before its word) does nothing.
    const std::string value = rows()[selected_].value;
    std::size_t start = 0u;
    for (unsigned i = 0u; i < button; ++i) start = value.find('|', start) + 1u;
    if (value[start] == '-') return;
    if (screen_ == Screen::Host) {
        press_host(selected_);
        return;
    }
    switch (selected_) {
    case RowHost: show(Screen::Host); break;
    case RowOptions: command_ = MenuCommand::Settings; break;
    case RowQuit: command_ = MenuCommand::Quit; break;
    default: break;
    }
}

void MainMenu::press_host(unsigned row) {
    switch (row) {
    case RowErase:
        if (!erase_armed_) {
            erase_armed_ = true;
            return;
        }
        erase_armed_ = false;
        if (saves_ != nullptr && save_ < save_ids_.size() && saves_->erase(save_ids_[save_])) {
            save_ids_.erase(save_ids_.begin() + save_);
            save_names_.erase(save_names_.begin() + save_);
        }
        return;
    case RowStart:
        chosen_save_ = save_ < save_ids_.size() ? save_ids_[save_] : std::string();
        command_ = MenuCommand::Play;
        return;
    case RowBack: back(); return;
    default: return;
    }
}

void MainMenu::back() {
    if (!open_ || screen_ == Screen::Main) return;
    show(Screen::Main);
}

MenuCommand MainMenu::take_command() noexcept {
    return std::exchange(command_, MenuCommand::None);
}

}  // namespace galaxy::coop

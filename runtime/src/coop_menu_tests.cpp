// The online co-op main menu's model (galaxy/coop_menu.h): its rows, moving between them, and what pressing does.
#include "galaxy/coop_menu.h"

#include <cstdio>

namespace {
int s_failures = 0;
void expect(bool ok, const char* what) {
    if (!ok) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++s_failures;
    }
}
}  // namespace

int main() {
    using galaxy::coop::MainMenu;
    using galaxy::coop::MenuCommand;

    MainMenu menu(true);
    expect(!menu.view().open, "closed until opened");
    menu.press(0u);
    expect(menu.take_command() == MenuCommand::None, "a closed menu takes no press");

    menu.open();
    galaxy::coop::MenuView view = menu.view();
    expect(view.open && view.page == 0u && view.pages.size() == 1u, "one page, open");
    expect(view.rows.size() == 4u && view.selected == 0u, "four rows, Host selected");
    expect(view.rows[0].label == "HOST" && view.rows[1].value == "-JOIN" && view.rows[2].value == "OPEN",
           "Host plays, Join unavailable, Options opens");

    menu.press(0u);
    expect(menu.take_command() == MenuCommand::Play, "Host plays");
    expect(menu.take_command() == MenuCommand::None, "a command is taken once");

    menu.move(1);
    expect(menu.view().selected == 1u, "down to Join");
    menu.press(0u);
    expect(menu.take_command() == MenuCommand::None, "Join is unavailable");
    menu.move(1);
    menu.press(0u);
    expect(menu.take_command() == MenuCommand::Settings, "Options opens the settings");
    menu.move(1);
    menu.press(0u);
    expect(menu.take_command() == MenuCommand::Quit, "Quit quits");
    menu.move(1);
    expect(menu.view().selected == 0u, "down from the last row wraps to the first");
    menu.move(-1);
    expect(menu.view().selected == 3u, "up from the first row wraps to the last");

    menu.select_row(9u);
    expect(menu.view().selected == 3u, "a row past the page is ignored");
    menu.press(1u);
    expect(menu.take_command() == MenuCommand::None, "a button the row doesn't have is ignored");
    menu.change(1);
    expect(menu.view().selected_button == 0u, "a one-button row has nothing to move to");

    MainMenu no_settings(false);
    no_settings.open();
    expect(no_settings.view().rows[2].value == "-OPEN", "Options unavailable without the in-game settings");
    no_settings.select_row(2u);
    no_settings.press(0u);
    expect(no_settings.take_command() == MenuCommand::None, "an unavailable Options does nothing");

    menu.close();
    expect(!menu.view().open, "closed");
    menu.open();
    expect(menu.view().selected == 0u, "reopening starts at Host");

    if (s_failures != 0) return 1;
    std::puts("Co-op menu tests passed");
    return 0;
}

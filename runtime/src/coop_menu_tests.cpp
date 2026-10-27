// The online co-op main menu's model (galaxy/coop_menu.h): its rows, moving between them, what pressing does, and the
// Host page over the co-op saves (galaxy/coop_saves.h).
#include "galaxy/coop_menu.h"
#include "galaxy/coop_saves.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>

namespace {
int s_failures = 0;
void expect(bool ok, const char* what) {
    if (!ok) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++s_failures;
    }
}

namespace fs = std::filesystem;

void write(const fs::path& path, const char* text) {
    std::ofstream(path, std::ios::binary) << text;
}

std::string read(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void test_saves(const fs::path& scratch) {
    using galaxy::coop::SaveStore;
    SaveStore store(scratch / "coop_saves");
    expect(store.list().empty() && !store.converted(), "no saves, not converted, before the folder exists");

    // The old saves: files 1 and 3 of six, and the achievements shared by all of them.
    const fs::path nand = scratch / "nand_data";
    fs::create_directories(nand);
    write(nand / "GameData.bin", "old");
    write(scratch / "achievements.ini", "star1=2026\n");
    expect(store.convert(nand, 0b101u, scratch / "achievements.ini"), "the old files convert");
    expect(store.converted() && !store.convert(nand, 0b101u, {}), "they convert once");
    std::vector<galaxy::coop::SaveInfo> saves = store.list();
    expect(saves.size() == 2u, "one save per old file");
    expect(saves.size() == 2u && saves[0].name == "FILE 3" && saves[0].file == 3u && saves[1].file == 1u,
           "the newest first, each playing its old file");
    expect(read(store.folder("save1") / "GameData.bin") == "old" &&
               read(store.folder("save2") / "achievements.ini") == "star1=2026\n",
           "each a copy of the old saves and achievements");
    expect(read(nand / "GameData.bin") == "old", "the old saves stay as they were");

    const std::string fresh = store.create();
    expect(
        fresh == "save3" && store.find(fresh) && store.find(fresh)->name == "SAVE 3" && store.find(fresh)->file == 1u,
        "a new save, numbered after the others, plays file 1");
    expect(store.mark_played("save1", 100) && store.list()[0].id == "save1", "the last played comes first");
    expect(store.erase("save2") && !store.find("save2") && store.list().size() == 2u, "erased");
    expect(store.create() == "save4", "an erased number isn't reused while a later one remains");
    expect(!store.erase("../nand_data") && fs::exists(nand), "an id never leaves the saves folder");
    expect(!store.find("save9"), "a missing save isn't found");

    SaveStore empty(scratch / "empty_saves");
    expect(empty.convert(scratch / "missing", 0u, {}) && empty.converted() && empty.list().empty(),
           "no old files: converted, with no saves");
}

void test_menu(const fs::path& scratch) {
    using galaxy::coop::MainMenu;
    using galaxy::coop::MenuCommand;

    MainMenu menu(true);
    expect(!menu.view().open, "closed until opened");
    menu.press(0u);
    expect(menu.take_command() == MenuCommand::None, "a closed menu takes no press");

    menu.open();
    galaxy::coop::MenuView view = menu.view();
    expect(view.open && view.page == 0u && view.pages.size() == 1u && view.pages[0] == "PLAY", "one page, open");
    expect(view.rows.size() == 4u && view.selected == 0u, "four rows, Host selected");
    expect(view.rows[0].label == "HOST" && view.rows[1].value == "-JOIN" && view.rows[2].value == "OPEN",
           "Host opens its page, Join unavailable, Options opens");

    menu.move(1);
    expect(menu.view().selected == 1u, "down to Join");
    menu.press(0u);
    expect(menu.take_command() == MenuCommand::None, "Join is unavailable");
    menu.move(1);
    menu.press(0u);
    expect(menu.take_command() == MenuCommand::Settings, "Options opens the settings");
    expect(menu.take_command() == MenuCommand::None, "a command is taken once");
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
    menu.back();
    expect(menu.view().pages[0] == "PLAY", "B on the main page stays");

    MainMenu no_settings(false);
    no_settings.open();
    expect(no_settings.view().rows[2].value == "-OPEN", "Options unavailable without the in-game settings");
    no_settings.select_row(2u);
    no_settings.press(0u);
    expect(no_settings.take_command() == MenuCommand::None, "an unavailable Options does nothing");

    // The Host page, over two saves.
    galaxy::coop::SaveStore store(scratch / "menu_saves");
    const std::string first = store.create("ONE");
    const std::string second = store.create("TWO");
    store.mark_played(first, 50);
    MainMenu host(true, &store);
    host.open();
    host.press(0u);
    expect(host.take_command() == MenuCommand::None, "Host opens its page, nothing to play yet");
    view = host.view();
    expect(view.pages[0] == "HOST" && view.rows.size() == 9u && view.selected == 0u, "the Host page, Save selected");
    expect(view.rows[0].value == "ONE" && view.rows[0].changes && view.rows[0].name, "the last played save first");
    host.change(1);
    expect(host.view().rows[0].value == "TWO", "right: the next save");
    host.change(1);
    expect(
        host.view().rows[0].value == "NEW SAVE" && !host.view().rows[0].name && host.view().rows[1].value == "-ERASE",
        "then a new one, which can't be erased");
    host.change(1);
    expect(host.view().rows[0].value == "NEW SAVE", "the new save is the last");
    host.change(2);
    expect(host.view().rows[0].value == "ONE", "a click on the row goes round to the first");
    host.change(-2);
    expect(host.view().rows[0].value == "NEW SAVE", "and back round to the last");
    host.change(-1);

    host.move(1);
    host.press(0u);
    expect(host.view().rows[1].value == "PRESS AGAIN TO ERASE" && store.find(second), "Erase asks first");
    host.press(0u);
    expect(!store.find(second) && host.view().rows[0].value == "NEW SAVE", "then erases: the new save is next");
    host.move(-1);
    host.change(-1);
    expect(host.view().rows[0].value == "ONE", "left: back to the save");

    const galaxy::RuntimeCoopSettings before = host.room();
    host.select_row(2u);
    host.change(1);
    host.select_row(6u);
    host.change(1);
    const galaxy::RuntimeCoopSettings after = host.room();
    expect(after.max_players == before.max_players + 1u && after.cheats, "the room's settings change");
    host.select_row(4u);
    host.change(1);
    host.change(1);
    expect(host.room().knockback_strength == galaxy::kCoopKnockbackTooMuch,
           "knockback stops at too much");

    host.select_row(7u);
    host.press(0u);
    expect(host.take_command() == MenuCommand::Play && host.chosen_save() == first, "Host plays the picked save");

    host.select_row(8u);
    host.press(0u);
    expect(host.view().pages[0] == "PLAY" && host.view().selected == 0u, "Back returns to the main page");
    host.press(0u);
    host.select_row(0u);
    host.change(1);
    host.select_row(7u);
    host.press(0u);
    expect(host.take_command() == MenuCommand::Play && host.chosen_save().empty(), "Host on New save: a new one");
    host.back();
    expect(host.view().pages[0] == "PLAY", "B returns to the main page");

    host.close();
    expect(!host.view().open, "closed");
    host.open();
    expect(host.view().selected == 0u && host.view().pages[0] == "PLAY", "reopening starts at Host");
}
}  // namespace

int main() {
    std::error_code error;
    const fs::path scratch = fs::temp_directory_path(error) / "galaxy_coop_menu_tests";
    fs::remove_all(scratch, error);
    fs::create_directories(scratch, error);
    test_saves(scratch);
    test_menu(scratch);
    fs::remove_all(scratch, error);

    if (s_failures != 0) return 1;
    std::puts("Co-op menu tests passed");
    return 0;
}

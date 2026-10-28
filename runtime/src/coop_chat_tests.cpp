// Online co-op's chat (galaxy/coop_chat.h): typing a line, what shows and for how long, floods, and lines between
// players in a room over the loopback.
#include "galaxy/coop_chat.h"
#include "galaxy/coop_menu.h"
#include "galaxy/net/bytes.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace {
int s_failures = 0;
void expect(bool ok, const char* what) {
    if (!ok) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++s_failures;
    }
}

using namespace std::chrono_literals;
using namespace galaxy::coop;

std::vector<std::uint8_t> message(const std::string& text) {
    galaxy::net::ByteWriter out;
    out.str(text);
    return out.bytes();
}

SessionPlayer player(galaxy::net::PeerId id, const char* name, galaxy::CoopCharacter character) {
    SessionPlayer p;
    p.id = id;
    p.name = name;
    p.character = character;
    return p;
}

void type_text(Chat& chat, const std::string& text, std::uint64_t now) {
    for (const char c : text) chat.type(static_cast<char32_t>(c), now);
}

void test_clean_text() {
    expect(clean_chat_text("  hi there  ") == "hi there", "spaces at a message's ends are dropped");
    expect(clean_chat_text("a\tb\nc\x01\xc3\xa9") == "abc", "only printable ASCII is kept");
    expect(clean_chat_text(std::string(300u, 'x')).size() == kChatMessageLimit, "a long message is cut");
    expect(clean_chat_text("   ").empty(), "a message of spaces is none");
}

void test_typing() {
    Chat chat;
    expect(!chat.typing() && !chat.type('a', 0u), "nothing types before the line opens");
    expect(chat.open() && chat.typing() && !chat.open(), "the line opens once");
    const std::uint32_t opened = chat.view(0u).input_serial;
    type_text(chat, "helo", 0u);
    chat.type(kTextLeft, 0u);
    chat.type('l', 0u);
    expect(chat.view(0u).input == "hello" && chat.view(0u).cursor == 4u, "typing goes in at the cursor");
    chat.type(kTextHome, 0u);
    chat.type(kTextDelete, 0u);
    chat.type(kTextEnd, 0u);
    chat.type(kTextBackspace, 0u);
    expect(chat.view(0u).input == "ell", "Delete and Backspace take a letter");
    expect(chat.view(0u).input_serial != opened, "the typed text's changes are counted");
    chat.type(0x263Au, 0u);
    expect(chat.view(0u).input == "ell", "a character the font lacks isn't typed");
    type_text(chat, std::string(200u, 'z'), 0u);
    expect(chat.view(0u).input.size() == kChatMessageLimit, "the line holds a message's longest");
    chat.type(kTextEscape, 0u);
    expect(!chat.typing() && chat.view(0u).input.empty(), "Escape drops the line");

    chat.open();
    type_text(chat, "   ", 0u);
    chat.type(kTextEnter, 0u);
    expect(!chat.typing() && chat.lines().empty(), "Enter on an empty line closes it, sending nothing");
    chat.open();
    type_text(chat, "alone", 0u);
    chat.type(kTextEnter, 0u);
    expect(!chat.typing() && chat.lines().empty(), "outside a room a line goes nowhere");
}

void test_lines() {
    Chat chat;
    const SessionPlayer luigi = player(5u, "LUIGI FAN", galaxy::CoopCharacter::Luigi);
    chat.receive(luigi, message("hello  "), 1000u);
    chat.notice(ChatLine::Kind::Joined, "NEW", 1500u);
    std::vector<ChatLine> lines = chat.lines();
    expect(lines.size() == 2u && lines[0].name == "LUIGI FAN" && lines[0].text == "hello" &&
               lines[0].character == galaxy::CoopCharacter::Luigi && lines[0].kind == ChatLine::Kind::Message,
           "a message shows under its sender's name and character");
    expect(lines.size() == 2u && lines[1].kind == ChatLine::Kind::Joined && lines[1].name == "NEW" &&
               lines[1].id != lines[0].id,
           "a join shows as its own line");

    chat.receive(luigi, message("   "), 1000u);
    chat.receive(luigi, std::vector<std::uint8_t>{0x05u}, 1000u);
    expect(chat.lines().size() == 2u, "an empty or broken message shows nothing");

    // Over play: shown, fading, gone.
    ChatView view = chat.view(1000u + kChatLineMs / 2u);
    expect(view.lines.size() == 2u && view.lines[0].alpha == 1.0f, "a new line shows");
    view = chat.view(1000u + kChatLineMs - kChatFadeMs / 2u);
    expect(view.lines.size() == 2u && view.lines[0].alpha > 0.4f && view.lines[0].alpha < 0.6f, "then fades");
    view = chat.view(1000u + kChatLineMs);
    expect(view.lines.size() == 1u && view.lines[0].line.name == "NEW", "then goes");
    chat.open();
    view = chat.view(1000000u);
    expect(view.typing && view.lines.size() == 2u && view.lines[0].alpha == 1.0f, "typing shows the last lines again");
    chat.close();

    // A flood: past its burst a player's lines are dropped, another's still come.
    Chat flooded;
    for (int i = 0; i < 20; ++i) flooded.receive(luigi, message("spam"), 50000u);
    flooded.receive(player(6u, "OTHER", galaxy::CoopCharacter::Mario), message("hi"), 50000u);
    expect(flooded.lines().size() == kChatBurstLines + 1u, "a flood is cut to its burst");
    flooded.receive(luigi, message("later"), 50000u + kChatBurstMs);
    expect(flooded.lines().back().text == "later", "and the player speaks again after it");

    for (int i = 0; i < 100; ++i) {
        flooded.receive(player(100u + static_cast<unsigned>(i), "MANY", galaxy::CoopCharacter::Mario), message("x"),
                        90000u);
    }
    expect(flooded.lines().size() == kChatLogLimit, "the log keeps its newest lines");
    expect(flooded.view(90000u).lines.size() == kChatShownLines, "and shows the last few");
    flooded.clear();
    expect(flooded.lines().empty() && !flooded.typing(), "a room's end clears it");
}

bool wait_for(const std::function<bool()>& done) {
    for (int i = 0; i < 1000; ++i) {
        if (done()) return true;
        std::this_thread::sleep_for(5ms);
    }
    return done();
}

void test_room() {
    // The chats outlive the sessions, whose threads give them lines.
    Chat host_chat, joiner_chat, third_chat;
    Session host_session, joiner, third;
    host_chat.attach(host_session);
    joiner_chat.attach(joiner);
    third_chat.attach(third);

    HostConfig host;
    host.port = 0u;
    host.game_version = "chat test";
    host.name = "HOSTY";
    host.save = [] { return RoomSave{}; };
    expect(host_session.host(host), "a room opens");
    const auto join = [&](const char* name, galaxy::CoopCharacter character) {
        JoinConfig config;
        config.address = "127.0.0.1";
        config.port = host_session.port();
        config.game_version = "chat test";
        config.name = name;
        config.character = character;
        return config;
    };
    expect(joiner.join(join("LUIGI FAN", galaxy::CoopCharacter::Luigi)), "a player joins");
    expect(third.join(join("THIRD", galaxy::CoopCharacter::Mario)), "another joins");
    expect(wait_for([&] {
               return joiner.state() == SessionState::Joined && third.state() == SessionState::Joined &&
                      joiner.players().size() == 3u && third.players().size() == 3u;
           }),
           "and everyone is in");

    joiner_chat.open();
    type_text(joiner_chat, "hi all", chat_clock_ms());
    joiner_chat.type(kTextEnter, chat_clock_ms());
    expect(!joiner_chat.typing(), "Enter sends the line");
    const std::vector<ChatLine> own = joiner_chat.lines();
    expect(own.size() == 1u && own[0].local && own[0].name == "LUIGI FAN" && own[0].text == "hi all",
           "and shows it as the sender's own");
    const auto said = [](const Chat& chat, const char* name, const char* text) {
        const std::vector<ChatLine> lines = chat.lines();
        return std::any_of(lines.begin(), lines.end(), [&](const ChatLine& line) {
            return line.name == name && line.text == text && !line.local &&
                   line.character == (std::string(name) == "LUIGI FAN" ? galaxy::CoopCharacter::Luigi
                                                                       : galaxy::CoopCharacter::Mario);
        });
    };
    expect(wait_for([&] { return said(host_chat, "LUIGI FAN", "hi all") && said(third_chat, "LUIGI FAN", "hi all"); }),
           "a player's line reaches the host and, through it, the others");

    host_chat.open();
    type_text(host_chat, "welcome", chat_clock_ms());
    host_chat.type(kTextEnter, chat_clock_ms());
    expect(wait_for([&] { return said(joiner_chat, "HOSTY", "welcome") && said(third_chat, "HOSTY", "welcome"); }),
           "the host's line reaches everyone");

    third.leave();
    joiner.leave();
    host_session.leave();
}

}  // namespace

int main() {
    test_clean_text();
    test_typing();
    test_lines();
    test_room();
    if (s_failures == 0) std::printf("coop chat tests passed\n");
    return s_failures == 0 ? 0 : 1;
}

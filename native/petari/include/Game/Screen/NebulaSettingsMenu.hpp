#pragma once

#ifdef TARGET_PC
// Nebula's settings menu in the game's own look: the quit question's white window over a dimmed screen, the game's
// buttons and message font. Its pages and rows are the host's (NebulaHostServices::settings_view: the settings, the
// draft and its apply), named in the played language. It drops in over anything (F1, View+Menu, Minus+Plus, or the
<<<<<<< HEAD
// pause menu's Settings) over a game that plays on, without the player's input until it has risen away again.
=======
// pause menu's Settings) and holds the whole game still as the HOME Menu does, its sound and movies with it, until it
// has risen away again.
>>>>>>> upstream/main
namespace NebulaSettingsMenu {
    void init();             // GameSystem, once the stationed layouts are loaded
    void update();           // GameSystem::update, once a frame
    void draw();             // GameSystem::draw, over the scene and its wipes
<<<<<<< HEAD
    bool isHoldingInput();   // open or opening or closing: the game plays on without the player's input
    bool isShown();          // drawn this frame (the pointer is drawn over it)
    void requestOpen();      // the pause menu's Settings
    void setMainMenuActive(bool active);  // the file select shows the online co-op main menu (FileSelector)
=======
    bool isFreezingScene();  // open or opening or closing: the scene waits
    bool isShown();          // drawn this frame (the pointer is drawn over it)
    void requestOpen();      // the pause menu's Settings
>>>>>>> upstream/main
};  // namespace NebulaSettingsMenu
#endif

# Controller checklist

For each controller on real hardware, before a release. The unit tests cover the report layouts and the mapping;
these checks cover what only a real controller shows. Mark each cell pass, fail (with a note) or n/a.

## Every controller

1. Connects: its name shows under CONTROLS > EDIT within a second (by each link it has: USB, Bluetooth, receiver).
2. Icons: the game's button icons and the settings menu's hints show its family's buttons.
3. Buttons: every row of the Default maps table in [KEYBOARD_CONTROLS.md](KEYBOARD_CONTROLS.md) does what it says,
   in play and in menus (B backs out of menus, jumps in play).
4. Sticks and triggers: full travel both ways, no drift at rest, crouch on a half pull.
5. Pointer: the right stick (and the gyro where it has one) moves it; recenter brings it to the middle.
6. Motion (where it has a gyro): shake spins; the Star Ball rolls with tilt (Rolling Green Galaxy) and the ray
   steers with it (Beach Bowl Galaxy, Surfing 101).
7. Vibration: a Star Bit shot and a hit buzz briefly; none in a menu, on pause or with the window in the background.
8. Hot-plug: unplugged (or switched off) in play, the game goes on; back, it works again within two seconds.
9. Two at once: two of this family, then this one with an Xbox controller: both move Mario; neither repeats a press.
10. Latency: a fast jump-spin-jump feels as immediate as with an Xbox controller.

## Families

| Controller | Links | Notes for the checks |
|---|---|---|
| Xbox Series / One / 360 | USB, Bluetooth, receiver | XInput; vibration on the light motor |
| DualShock 4 | USB, Bluetooth | gyro; light bar set once |
| DualSense, DualSense Edge | USB, Bluetooth | gyro; Edge paddles unbound by default |
| Switch Pro Controller | USB, Bluetooth | gyro, HD rumble |
| Joy-Con (L) + (R) | Bluetooth, charging grip | pair by themselves; CONTROLS > PAIR JOY-CON with two pairs; pointer from the right half; both vibrate; slide one into the grip after it is plugged in |
| Steam Controller (2026) | USB, Bluetooth, puck | no mouse or keyboard from it; gyro |
| Steam Deck | built in | trackpads not the mouse, R2/L2 not mouse buttons, for minutes on end (the watchdog); gyro; vibration |
| Steam Controller (2015) | USB, receiver | right pad is the pointer; left pad clicks are the D-pad; no mouse; pad haptics as vibration |
| Steam Input on | any of the above, started from Steam | the log says "left to Steam Input"; every press once |
| 8BitDo | X-input, Switch, D-input modes | X: as Xbox; Switch: as Pro (motion if the pad has it); D: rebind on CONTROLS |
| Generic HID pad | USB | buttons by number; rebind on CONTROLS |
| Wii Remote (+ Nunchuk) | Bluetooth | read directly; IR pointer, speaker sounds, its motor |

The log lines to look for start with `[controller]`: the scan, each controller connected or disconnected, and a
Joy-Con pair made.

# Legally-distinct-platformer-maker

# About
Legally-distinct-platformer maker is a mario maker inspired game with a custom platforming,enemy,collision and tiling engine I made with C and raylib for the graphics, sound and input. It currently features 8 different blocks and 1 enemy, as well as a level editor that is intuitive and fast to prototype, with shortcuts and tools such as rectangle fill, along with an 8x8 charm.

# Blocks
As mentioned, there are 8 block types and 1 enemy - here is all that you need to know to start making levels

| Block name | Features | Internal ID|
|------------|----------|-------------|
| Air/delete | deletes the selected block | 0|
| Grass      | a solid block on all 4 sides  | 1
| Brick      |  a solid block on all 4 sides | 2
| Start flag | a block that determines the player's start position - level must have one to start the level| 3
| End flag | A block that completes the level - level must have one so it can be ended| 4
| Spike | A spike that causes the player to go back to the start position when touched | 5
| Red cycle block | A block that is solid when blue is inactive and not solid when blue is active| 6
|Blue cycle block | A block that is solid when red is inactive and not solid when red is active | 7
|Coin| Plays a sound when collected, then disappears | 8
| Hedgehog (enemy) | An enemy that moves in the opposite direction when facing a wall, but does not move away from pits | 9

# Editor controls

| Control | Action|
|---------|-------|
| Left click | Places a block |
| Right click | Removes a block |
| Mouse zoom | Zooms in and out |
| Shift + left click (hold) | Allows to drag in a  direction |
|R| Toggles a rectangle fill mode |
| P (while in editor) | Starts the level |
| E (while in level) | Goes back to editor |

# Building
This project uses [raylib](https://www.raylib.com/) as the graphics library. A makefile is provided so you can build with `make`


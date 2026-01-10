# omuGen/gameoflife

A rough and badly optimized implementation of Conway's Game of Life in C++ using SDL2 and CMake (by a beginner in all three).

### Controls

'space' to pause the game.\
's' to slow down.\
'f' to run faster.\
'r' to reset to random board.\
'c' to clear the board.\
'f' to save the current board to file "save.txt".\
'l' to load a board configuration from file "save.txt".

### CLI arguments

--rulestring "xx" Allows rule configuration. Example: --rulestring "B3S23"\
--random xx Sets the randomizer to the specified percent value. Example: --random 25

## Version History
| x.x | changes |
| - | - |
| 0.3 | cli arguments for rules and randomness; simple save/load system; code cleanup and optimization |
| 0.2 | basic features implemented; performance adequately stable |
| 0.1 | keyboard, mouse interaction; direct mouse cell manipulation; pause, reset, clear features; testing performance issues|
| 0.1< | rough outline; game implemented; SDL API basics; CMake woes; ...|
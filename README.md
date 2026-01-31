Space Invader – C Terminal Game
A classic Space Invader style arcade game built in C using the ncurses library.Runs directly in the terminal with real-time keyboard controls, animated enemies, and score tracking.This project demonstrates low-level game logic, console rendering, and event-driven input handling.

Features
  1.Terminal-based arcade gameplay
  2.Real-time keyboard controls
  3.Enemy movement and attack patterns
  4.Player shooting mechanics
  5.Score tracking system
  6.Game loop with collision detection
  7.Menu / title screen
  8.Ncurses-powered screen rendering
  
Requirements
  - GCC compiler
  - ncurses library
  - Linux / Unix-like OS (recommended)
  - Install ncurses

Install ncurses
  sudo dnf install ncurses-devel      # Fedora
  sudo apt install libncurses5-dev    # Ubuntu/Debian

Compile & Run
gcc spaceinvader.c -o spaceinvader -lncurses
./spaceinvader

Controls
  ← →   Move player
  SPACE Fire
  Q     Quit

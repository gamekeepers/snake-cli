# Snake Game 🐍

A feature-rich console Snake game with scoring, difficulty scaling, poisonous food, pause functionality, and high score tracking.

## Features ✨
- 🍎 **Food System**: Eat food to grow and increase score (+10 points)
- ☠️ **Poisonous Food**: Avoid poison or game over!
- 📈 **Dynamic Difficulty**: Speed increases as you score more
- ⏸️ **Pause/Resume**: Press 'p' to pause the game
- 🏆 **High Scores**: Top 10 scores saved automatically
- 🎯 **Smart Spawning**: Food never spawns inside snake body

## Compile and Run
```cmd
g++ -o game.exe main.cpp
game.exe
```

## Controls 🎮
- `w` - Move Up
- `s` - Move Down  
- `a` - Move Left
- `d` - Move Right
- `p` - Pause/Resume
- `q` - Quit Game

## Run Tests
```cmd
g++ -o tests.exe snake_test.cpp -lgtest -lgtest_main
tests.exe
```

## Files 📁
- `main.cpp` - Game entry point
- `snake.h` - Core game logic
- `snake_test.cpp` - Unit tests
- `scores.txt` - High scores (auto-created)
- `game.exe` - Compiled executable

# Snake Game

A classic snake game implemented in C++.

## Features

*   Classic snake gameplay
*   Keyboard controls
*   Threaded input and game logic
*   Unit tests using Google Test framework

- **Three difficulty levels**: Easy, Medium, and Hard (affects speed and score multiplier)
- **Score tracking**: Earn points based on difficulty (Easy: 10pts, Medium: 20pts, Hard: 30pts per food)
- **Pause functionality**: Press 'P' to pause/resume the game
- **Clean game over screen**: Shows final score, snake length, and difficulty
- **Real-time stats display**: Score, length, and difficulty shown during gameplay

## Getting Started

### Prerequisites

*   A C++ compiler (like g++)
*   CMake (version 3.10 or higher)
*   Google Test library

### Building the Project

1.  Create a build directory:
    ```bash
    mkdir build
    ```
2.  Navigate to the build directory:
    ```bash
    cd build
    ```
3.  Run CMake to configure the project:
    ```bash
    cmake ..
    ```
4.  Compile the project:
    ```bash
    make
    ```

This will create two executables in the `build` directory: `snake_game` and `snake_tests`.

### Running the Game

To play the game, run the following command from the `build` directory:

```bash
g++ -o snake main.cpp snake.cpp -pthread
./snake
```

You can also specify difficulty via command line (1=Easy, 2=Medium, 3=Hard):
```bash
./snake 2  # Start with Medium difficulty
```

### Running Tests

To run the tests, execute the following command from the `build` directory:

```bash
g++ -o snake_tests snake_test.cpp snake_input_test.cpp snake.cpp  -lgtest -lgtest_main -pthread
./my_tests
```

## How to Play

Use the WASD keys to control the snake's direction. The goal is to eat the food that appears on the screen, which makes the snake grow longer. The game ends if the snake runs into the wall or into itself.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
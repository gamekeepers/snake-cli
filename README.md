# Snake Game

A classic snake game implemented in C++.

## Features

*   Classic snake gameplay
*   Keyboard controls
*   Threaded input and game logic
*   Unit tests using Google Test framework

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
./snake_game
```

### Running Tests

To run the tests, execute the following command from the `build` directory:

```bash
./snake_tests
```

## How to Play

Use the WASD keys to control the snake's direction. The goal is to eat the food that appears on the screen, which makes the snake grow longer. The game ends if the snake runs into the wall or into itself.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
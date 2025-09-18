#include "snake.h"
#include <iostream>

int main() {
    std::cout << "Starting Snake Game..." << std::endl;
    std::cout << "Controls: W (Up), A (Left), S (Down), D (Right), Q (Quit)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Game game;
    game.run();

    return 0;
}
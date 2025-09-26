#include <thread>
#include "snake.h"

int main(int argc, char *argv[]) {
    int boardSize = 8;
    if (argc > 1) {
        int requestedSize = atoi(argv[1]);
        if (requestedSize == 7 || requestedSize == 8) {
            boardSize = requestedSize;
        }
    }
    Game snakeGame(boardSize);
    std::thread inputThread(&Game::runInput, &snakeGame);
    std::thread gameThread(&Game::runLoop, &snakeGame);
    inputThread.join();
    gameThread.join();
    return 0;
}

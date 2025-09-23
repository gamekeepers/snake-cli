#include <thread>
#include "snake.h"

int main(int argc, char *argv[]) {
    int size = 8;
    if (argc > 1) {
        int requested = atoi(argv[1]);
        if (requested == 7 || requested == 8) {
            size = requested;
        }
    }
    Game game(size);
    std::thread input_thread(&Game::runInput, &game);
    std::thread game_thread(&Game::runLoop, &game);
    input_thread.join();
    game_thread.join();
    return 0;
}
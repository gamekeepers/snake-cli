#include "snake.h"
#include <thread>

int main(int argc, char *argv[]) {
    // Start input thread first so it can capture keys
    std::thread input_thread(input_handler);
    input_thread.detach(); // run in background

    // Run the game (this will return when game ends)
    game_play();

    return 0;
}

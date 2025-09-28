#include "snake.h"
#include <thread>

int main() {
    Game game;
    thread input_thread(&Game::handle_input, &game);
    thread game_thread(&Game::play, &game);

    input_thread.join();
    game_thread.join();

    return 0;
}

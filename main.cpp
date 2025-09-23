#include "snake.h"

int main(int argc, char *argv[]) {
    Game game;
    thread input_thread(&Game::input_handler, &game);
    thread game_thread(&Game::play, &game);

    input_thread.join();
    game_thread.join();
    return 0;
}

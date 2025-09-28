#include "snake.h"

int main(int argc, char *argv[]) {
    SnakeGame game;
    thread input_thread(&SnakeGame::input_handler, &game);
    thread game_thread(&SnakeGame::game_play, &game);
    input_thread.join();
    game_thread.join();
    return 0;
}
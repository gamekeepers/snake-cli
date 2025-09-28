#include "snake.h"

int main(int argc, char *argv[]) {
    GameState state;
    thread input_thread(input_handler, std::ref(state));
    thread game_thread(game_play, std::ref(state));
    input_thread.join();
    game_thread.join();
    return 0;
}
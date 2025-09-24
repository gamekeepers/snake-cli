#include "snake.h"

int main(int argc, char *argv[]) {
    Game game = Game();

    thread input_thread(input_handler, std::ref(game));
    thread game_thread(game_play, std::ref(game));   
    input_thread.join();
    game_thread.join();
    return 0;
}
#include "snake.h"

int main() {
    srand(time(0)); // seed random generator

    select_difficulty();

    thread input_thread(input_handler);
    thread game_thread(game_play);

    input_thread.join();
    game_thread.join();

    return 0;
}

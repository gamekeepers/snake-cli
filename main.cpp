#include "snake.h"

int main(int argc, char *argv[]) {
    Snake snake = Snake();

    thread input_thread(input_handler, std::ref(snake));
    thread game_thread(game_play, std::ref(snake));   
    input_thread.join();
    game_thread.join();
    return 0;
}
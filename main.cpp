#include "snake.h"

int main(int argc, char *argv[]) {
    thread input_thread(input_handler);
    thread game_thread(game_play);   
    input_thread.join();
    game_thread.join();
return 0;
}
#include "snake.h"
#include <ctime>

int main(int argc, char *argv[])
{
    srand((unsigned)time(nullptr));

    thread input_thread(input_handler);
    thread game_thread(game_play);

    input_thread.join();
    game_thread.join();
    return 0;
}
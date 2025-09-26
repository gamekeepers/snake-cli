

#include "snake.h"
#include <ctime>
#include <thread>

char direction = 'r';

int main(int argc, char *argv[])
{
    thread input_thread(input_handler);
    game_play();
    input_thread.join();
    return 0;
}
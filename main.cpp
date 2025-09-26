#include "snake.h"
#include <thread>

char direction = 'r'; // initialize global direction

int main()
{
    thread inputThread(input_handler);
    game_play();
    inputThread.join();
    return 0;
}
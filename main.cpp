#include "snake.h"
#include <thread>

int main()
{
    std::thread input_thread(input_handler);

    game_play();
    
    input_thread.join();

    return 0;
}
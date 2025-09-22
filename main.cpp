#include "snake.h"
#include <thread>
using namespace std;

int main(int argc, char *argv[]) {
    system("clear");

    HighScores highScores;          
    Game game(10, highScores);      

    // start input handler thread and game thread
    thread input_thread(inputHandler, ref(game));
    thread game_thread(&Game::loop, &game);

    input_thread.join();
    game_thread.join();
    return 0;
}

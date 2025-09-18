#ifndef SNAKE_H
#define SNAKE_H
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h> // for system clear
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;


class Game {
public:
    Game(int size = 10);
    void run();

private:
    void spawn_food(); 
    void render();
    void update();
    bool check_collision();
    void game_over();
    std::pair<int, int> get_next_head();
    int score;
    int size;
    char direction;
    std::deque<std::pair<int, int>> snake;
    std::pair<int, int> food;
    bool is_running;
    std::chrono::milliseconds game_speed_ms;

    friend void input_handler_thread(Game* game);
};

void input_handler_thread(Game* game);

#endif // SNAKE_H


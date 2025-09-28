#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <map>
#include <deque>
#include <algorithm>

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
public:
    Snake(int board_size);
    void move();
    void grow();
    bool check_collision();
    Point get_head();
    const deque<Point>& get_body() const;
    void set_direction(char new_direction);
    char get_direction();

private:
    deque<Point> body;
    char direction;
    int board_size;
    Point get_next_head();
};

class Food {
public:
    Food(int board_size, const deque<Point>& snake_body);
    void generate_new_food(const deque<Point>& snake_body);
    Point get_position();

private:
    Point position;
    int board_size;
};

class Game {
public:
    Game(int size);
    void run();

private:
    void render();
    void update();
    void process_input();

    int board_size;
    Snake snake;
    Food food;
    bool game_over;
    thread input_thread;
};
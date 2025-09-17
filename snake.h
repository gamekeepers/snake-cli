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
#include <utility>
using namespace std;
using std::chrono::system_clock;

constexpr int BOARD_SIZE = 10;   // board is BOARD_SIZE x BOARD_SIZE

char direction = 'r';

// input handler (runs in separate thread)
void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // turn off canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        } else if (input == 'q') {
            // restore terminal settings before quitting
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            exit(0);
        }
    }
    // unreachable, but keep for symmetry
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// render the board
void render_game(int size, deque<pair<int,int>> &snake, pair<int,int> food) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i == food.first && j == food.second) {
                cout << "🍎";
            } else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << '\n';
    }
}

// compute next head (wraps around)
pair<int,int> get_next_head(pair<int,int> current, char direction) {
    pair<int,int> next;
    if (direction == 'r') {
        next = make_pair(current.first, (current.second + 1) % BOARD_SIZE);
    } else if (direction == 'l') {
        next = make_pair(current.first, current.second == 0 ? BOARD_SIZE - 1 : current.second - 1);
    } else if (direction == 'd') {
        next = make_pair((current.first + 1) % BOARD_SIZE, current.second);
    } else { // 'u'
        next = make_pair(current.first == 0 ? BOARD_SIZE - 1 : current.first - 1, current.second);
    }
    return next;
}

// spawn food at a location not occupied by the snake
pair<int,int> spawn_food(const deque<pair<int,int>> &snake) {
    // if snake fills board, return invalid food
    if ((int)snake.size() >= BOARD_SIZE * BOARD_SIZE) {
        return make_pair(-1, -1);
    }
    pair<int,int> f;
    do {
        f.first = rand() % BOARD_SIZE;
        f.second = rand() % BOARD_SIZE;
    } while (find(snake.begin(), snake.end(), f) != snake.end());
    return f;
}

void game_play() {
    system("clear");
    deque<pair<int,int>> snake;
    snake.push_back(make_pair(0, 0)); // starting single-segment snake

    pair<int,int> food = spawn_food(snake);

    while (true) {
        // compute next head from current head (snake.back())
        pair<int,int> currentHead = snake.back();
        pair<int,int> nextHead = get_next_head(currentHead, direction);

        // send cursor to top-left so we overwrite previous frame
        cout << "\033[H";

        bool willGrow = (nextHead == food);
        bool collision = false;

        if (willGrow) {
            // If eating, tail won't move — colliding with any part is fatal
            if (find(snake.begin(), snake.end(), nextHead) != snake.end()) {
                collision = true;
            }
        } else {
            // Not growing: tail will be popped, so moving into current tail is allowed.
            // Check collision against body excluding the tail (i.e., start from snake.begin()+1)
            auto itStart = snake.begin();
            if (!snake.empty()) ++itStart; // skip tail
            if (find(itStart, snake.end(), nextHead) != snake.end()) {
                collision = true;
            }
        }

        if (collision) {
            system("clear");
            cout << "Game Over\n";
            cout << "Final length: " << snake.size() << "\n";
            exit(0);
        }

        // perform move
        snake.push_back(nextHead);
        if (willGrow) {
            // generate new food outside the snake
            food = spawn_food(snake);
            if (food.first == -1) {
                // board full → player wins
                system("clear");
                cout << "You Win! Final length: " << snake.size() << "\n";
                exit(0);
            }
        } else {
            snake.pop_front();
        }

        // draw
        render_game(BOARD_SIZE, snake, food);
        cout << "length of snake: " << snake.size() << '\n';

        // dynamic speed (harder as snake grows). floor at 100ms.
        int delay_ms = max(100, 500 - (int)snake.size() * 20);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}
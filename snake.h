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
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// render the board
void render_game(int size, deque<pair<int,int>> &snake, pair<int,int> food, int score) {
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
    cout << "Length: " << snake.size() << "  Score: " << score << "\n";
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

// main game loop
void game_play() {
    system("clear");
    deque<pair<int,int>> snake;
    snake.push_back(make_pair(0, 0)); // starting single-segment snake

    pair<int,int> food = spawn_food(snake);
    int score = 0;

    while (true) {
        // compute next head
        pair<int,int> currentHead = snake.back();
        pair<int,int> nextHead = get_next_head(currentHead, direction);

        // move cursor to top-left
        cout << "\033[H";

        bool willGrow = (nextHead == food);
        bool collision = false;

        if (willGrow) {
            if (find(snake.begin(), snake.end(), nextHead) != snake.end()) {
                collision = true;
            }
        } else {
            auto itStart = snake.begin();
            if (!snake.empty()) ++itStart; // skip tail
            if (find(itStart, snake.end(), nextHead) != snake.end()) {
                collision = true;
            }
        }

        if (collision) {
            system("clear");
            cout << "Game Over\n";
            cout << "Final Length: " << snake.size() << "  Final Score: " << score << "\n";
            exit(0);
        }

        // move/grow snake
        snake.push_back(nextHead);
        if (willGrow) {
            score += 1; // increase score when eating food
            food = spawn_food(snake);
            if (food.first == -1) {
                system("clear");
                cout << "You Win! Final Length: " << snake.size() << "  Final Score: " << score << "\n";
                exit(0);
            }
        } else {
            snake.pop_front();
        }

        // render
        render_game(BOARD_SIZE, snake, food, score);

        // dynamic speed: harder as snake grows
        int delay_ms = max(100, 500 - (int)snake.size() * 20);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}
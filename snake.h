#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>  // for system clear
#include <map>
#include <deque>
#include <algorithm>
#include <atomic>

using namespace std;
using namespace std::chrono_literals;
using std::this_thread::sleep_for;

// Using atomic to safely share direction across threads
inline atomic<char> direction{'r'};

namespace snake_game {

// Terminal settings helper class to manage termios RAII style
class TerminalSettings {
    struct termios oldt;
    bool changed = false;
public:
    TerminalSettings() {
        tcgetattr(STDIN_FILENO, &oldt);
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        changed = true;
    }
    ~TerminalSettings() {
        if (changed) {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        }
    }
};

// Input handler to capture keypresses asynchronously
void input_handler() {
    TerminalSettings term;  // ensures settings are restored on exit

    map<char, char> keymap = {
        {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}
    };

    while (true) {
        char input = getchar();
        if (keymap.count(input)) {
            char new_dir = keymap[input];
            if (new_dir == 'q') {
                exit(0);
            }
            // Prevent snake from reversing on itself
            char current = direction.load();
            if (!((current == 'r' && new_dir == 'l') ||
                  (current == 'l' && new_dir == 'r') ||
                  (current == 'u' && new_dir == 'd') ||
                  (current == 'd' && new_dir == 'u'))) {
                direction.store(new_dir);
            }
        }
    }
}

// Render the game board to the terminal
void render_game(int size, const deque<pair<int,int>>& snake, pair<int,int> food) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
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

// Calculate the next head position based on current direction and position
pair<int,int> get_next_head(pair<int,int> current, char dir, int board_size=10) {
    int x = current.first;
    int y = current.second;

    switch(dir) {
        case 'r': y = (y + 1) % board_size; break;
        case 'l': y = (y == 0) ? board_size - 1 : y - 1; break;
        case 'd': x = (x + 1) % board_size; break;
        case 'u': x = (x == 0) ? board_size - 1 : x - 1; break;
    }
    return make_pair(x, y);
}

// Generate a random food position not on the snake
pair<int,int> generate_food(int size, const deque<pair<int,int>>& snake) {
    pair<int,int> food;
    do {
        food = make_pair(rand() % size, rand() % size);
    } while (find(snake.begin(), snake.end(), food) != snake.end());
    return food;
}

void game_play(int board_size = 10) {
    system("clear");
    deque<pair<int, int>> snake = { {0,0} };
    pair<int,int> food = generate_food(board_size, snake);
    int food_eaten = 0;
    auto sleep_duration = 500ms;

    pair<int,int> head = {0,1};

    while (true) {
        cout << "\033[H";  // move cursor to top-left

        head = get_next_head(head, direction.load(), board_size);

        // Check self-collision
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over\n";
            exit(0);
        }

        if (head == food) {
            snake.push_back(head);
            food_eaten++;
            food = generate_food(board_size, snake);

            // Speed up every 10 food eaten, min 100ms
            if (food_eaten % 10 == 0 && sleep_duration > 100ms) {
                sleep_duration -= 50ms;
            }
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(board_size, snake, food);
        cout << "Length of snake: " << snake.size() << '\n';
        cout << "Speed: " << sleep_duration.count() << " ms per frame\n";

        sleep_for(sleep_duration);
    }
}

} // namespace snake_game

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <conio.h>   // Windows input
#include <windows.h> // Windows clear screen
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;
char direction = 'r';

// Windows: clear screen
void clear_screen() {
    HANDLE hOut;
    COORD Position;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    Position.X = 0;
    Position.Y = 0;
    SetConsoleCursorPosition(hOut, Position);
}

// Windows input handler
void input_handler() {
    map<char, char> keymap = {
        {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}
    };
    while (true) {
        if (_kbhit()) { // agar koi key press hui
            char input = _getch();
            if (keymap.find(input) != keymap.end()) {
                direction = keymap[input];
            } else if (input == 'q') {
                exit(0);
            }
        }
        sleep_for(50ms);
    }
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food) {
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "O"; // Food
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "X"; // Snake
            } else {
                cout << ".";
            }
        }
        cout << endl;
    }
}

pair<int, int> get_next_head(pair<int, int> current, char direction) {
    pair<int, int> next;
    if (direction == 'r') {
        next = make_pair(current.first, (current.second + 1) % 10);
    } else if (direction == 'l') {
        next = make_pair(current.first, current.second == 0 ? 9 : current.second - 1);
    } else if (direction == 'd') {
        next = make_pair((current.first + 1) % 10, current.second);
    } else if (direction == 'u') {
        next = make_pair(current.first == 0 ? 9 : current.first - 1, current.second);
    }
    return next;
}

void game_play() {
    system("cls"); // Windows clear
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = make_pair(rand() % 10, rand() % 10);
    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction)) {
        clear_screen();
        // check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("cls");
            cout << "Game Over" << endl;
            exit(0);
        } else if (head.first == food.first && head.second == food.second) {
            // grow snake
            food = make_pair(rand() % 10, rand() % 10);
            snake.push_back(head);
        } else {
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }
        render_game(10, snake, food);
        cout << "length of snake: " << snake.size() << endl;

        sleep_for(500ms);
    }
}

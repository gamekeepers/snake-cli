#ifndef SNAKE_H
#define SNAKE_H

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
#include <ctime>

using namespace std;
using namespace std::this_thread;
using std::chrono::system_clock;

char direction = 'r';
bool paused = false;
int score = 0;
vector<int> highScores;


void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};

    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            if (!paused) direction = keymap[input];
        } else if (input == 'p') {
            paused = !paused; // Toggle pause
        } else if (input == 'q') {
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";
            } else if (i == poison.first && j == poison.second) {
                cout << "☠️";
            } else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
    cout << "Length of snake: " << snake.size() << endl;
    cout << "[p] Pause/Resume | [q] Quit" << endl;
}


pair<int,int> get_next_head(pair<int,int> current, char direction, int size) {
    pair<int, int> next; 
    if (direction == 'r') {
        next = make_pair(current.first, (current.second + 1) % size);
    } else if (direction == 'l') {
        next = make_pair(current.first, current.second == 0 ? size - 1 : current.second - 1);
    } else if (direction == 'd') {
        next = make_pair((current.first + 1) % size, current.second);
    } else if (direction == 'u') {
        next = make_pair(current.first == 0 ? size - 1 : current.first - 1, current.second);
    }
    return next;
}


pair<int,int> spawn_food(int size, deque<pair<int,int>> &snake) {
    pair<int, int> food;
    do {
        food = make_pair(rand() % size, rand() % size);
    } while (find(snake.begin(), snake.end(), food) != snake.end());
    return food;
}


void game_play() {
    srand(time(0));
    int size = 10;
    int speed = 500; // in ms
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    pair<int, int> food = spawn_food(size, snake);
    pair<int, int> poison = spawn_food(size, snake);

    for (pair<int, int> head = make_pair(0,1); ; head = get_next_head(head, direction, size)) {
        cout << "\033[H";

        if (paused) {
            cout << "⏸️ Game Paused ⏸️" << endl;
            sleep_for(300ms);
            continue;
        }

        // Collision with self
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "💀 Game Over 💀" << endl;
            cout << "Final Score: " << score << endl;

            highScores.push_back(score);
            sort(highScores.rbegin(), highScores.rend());
            if (highScores.size() > 10) highScores.resize(10);

            cout << "\n🏆 Top 10 High Scores 🏆\n";
            for (size_t i = 0; i < highScores.size(); i++) {
                cout << i+1 << ". " << highScores[i] << endl;
            }
            exit(0);
        } 
        // Eating food
        else if (head.first == food.first && head.second == food.second) {
            score += 10;
            food = spawn_food(size, snake);
            poison = spawn_food(size, snake);
            snake.push_back(head);

            if (speed > 100) speed -= 20; // Increase difficulty
        } 
        // Eating poison
        else if (head.first == poison.first && head.second == poison.second) {
            system("clear");
            cout << "☠️ Game Over (Ate Poison) ☠️" << endl;
            cout << "Final Score: " << score << endl;
            exit(0);
        } 
        else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(size, snake, food, poison);
        sleep_for(std::chrono::milliseconds(speed));
    }
}

#endif

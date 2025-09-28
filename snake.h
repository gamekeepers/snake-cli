#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <conio.h>
#include <map>
#include <deque>
#include <algorithm>
#include <atomic>
#include <fstream>
#include <ctime>

// Prevent Windows header conflicts
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define byte win_byte_override
#include <windows.h>
#undef byte

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

atomic<char> direction('r');
atomic<bool> paused(false);

const int BOARD_SIZE = 10;  // board size
const string GREEN_BG = "\033[42m";
const string RED_BG   = "\033[41m";
const string BLUE_BG  = "\033[44m";
const string RESET    = "\033[0m";

vector<int> high_scores;

void load_high_scores() {
    ifstream in("highscores.txt");
    int s;
    while (in >> s) {
        high_scores.push_back(s);
    }
    sort(high_scores.rbegin(), high_scores.rend());
    if (high_scores.size() > 10) {
        high_scores.resize(10);
    }
}

void save_high_scores() {
    ofstream out("highscores.txt");
    for (int s : high_scores) {
        out << s << endl;
    }
}

void display_high_scores() {
    cout << "Top 10 Highest Scores:" << endl;
    for (size_t i = 0; i < high_scores.size(); ++i) {
        cout << (i + 1) << ". " << high_scores[i] << endl;
    }
}

void input_handler(atomic<bool>& running) {
    while (running) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 224 || ch == 0) {  // Extended key for arrows
                ch = _getch();
                switch (ch) {
                    case 72: direction = 'u'; break;  // Up
                    case 80: direction = 'd'; break;  // Down
                    case 75: direction = 'l'; break;  // Left
                    case 77: direction = 'r'; break;  // Right
                }
            } else {
                if (ch == 27) {  // ESC
                    running = false;
                    break;
                } else if (ch == ' ') {  // Space for pause/resume
                    paused = !paused.load();
                }
            }
        }
        sleep_for(10ms);  // Small sleep to prevent CPU hogging
    }
}

void render_game(const deque<pair<int, int>>& snake, pair<int, int> food, pair<int, int> poison, int score) {
    cout << "\033[H";  // Move cursor to top-left (instead of clearing)

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (i == food.first && j == food.second) {
                cout << GREEN_BG << "  " << RESET;
            } else if (i == poison.first && j == poison.second) {
                cout << RED_BG << "  " << RESET;
            } else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) {
                cout << GREEN_BG << "  " << RESET;
            } else {
                cout << BLUE_BG << "  " << RESET;
            }
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
}

pair<int, int> get_next_head(pair<int, int> current, char dir) {
    pair<int, int> next;
    if (dir == 'r') {
        next = make_pair(current.first, (current.second + 1) % BOARD_SIZE);
    } else if (dir == 'l') {
        next = make_pair(current.first, (current.second == 0 ? BOARD_SIZE - 1 : current.second - 1));
    } else if (dir == 'd') {
        next = make_pair((current.first + 1) % BOARD_SIZE, current.second);
    } else if (dir == 'u') {
        next = make_pair((current.first == 0 ? BOARD_SIZE - 1 : current.first - 1), current.second);
    }
    return next;
}

void game_play(atomic<bool>& running) {
    srand(static_cast<unsigned int>(time(NULL)));
    load_high_scores();

    // Hide cursor for smooth animation
    cout << "\033[?25l";

    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food;
    do {
        food = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
    } while (find(snake.begin(), snake.end(), food) != snake.end());

    pair<int, int> poison;
    do {
        poison = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
    } while (find(snake.begin(), snake.end(), poison) != snake.end() || poison == food);

    int score = 0;
    bool game_over = false;

    pair<int, int> head = get_next_head(snake.back(), direction);
    while (running) {
        if (find(snake.begin(), snake.end(), head) != snake.end() || head == poison) {
            game_over = true;
            break;
        }
        if (head == food) {
            snake.push_back(head);
            do {
                food = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
            } while (find(snake.begin(), snake.end(), food) != snake.end() || food == poison);
            do {
                poison = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
            } while (find(snake.begin(), snake.end(), poison) != snake.end() || poison == food);
            score = static_cast<int>(snake.size()) - 1;
        } else {
            snake.push_back(head);
            snake.pop_front();
        }
        render_game(snake, food, poison, score);

        while (paused) {
            sleep_for(50ms);
            if (!running) break;
        }
        if (!running) break;

        int sleep_ms = 500 - score * 20;
        if (sleep_ms < 100) sleep_ms = 100;
        sleep_for(chrono::milliseconds(sleep_ms));

        head = get_next_head(snake.back(), direction);
    }

    if (game_over) {
        high_scores.push_back(score);
        sort(high_scores.rbegin(), high_scores.rend());
        if (high_scores.size() > 10) high_scores.resize(10);
        save_high_scores();

        cout << "\033[H"; // Reset cursor to top-left
        cout << "Game Over!" << endl;
        display_high_scores();
    }

    // Show cursor back before exit
    cout << "\033[?25h";

    running = false;
}

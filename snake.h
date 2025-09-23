#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <map>
#include <deque>
#include <algorithm>
#include <atomic>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;
using namespace std::chrono_literals;
using namespace std::this_thread;

class Snake {
public:
    explicit Snake(int gridSize)
        : gridSize_(gridSize) {
        body_.push_back(make_pair(0, 0));
    }

    static pair<int, int> computeNextHead(const pair<int, int>& current, char direction, int gridSize) {
        pair<int, int> next = current;
        if (direction == 'r') {
            next = make_pair(current.first, (current.second + 1) % gridSize);
        } else if (direction == 'l') {
            next = make_pair(current.first, current.second == 0 ? gridSize - 1 : current.second - 1);
        } else if (direction == 'd') {
            next = make_pair((current.first + 1) % gridSize, current.second);
        } else if (direction == 'u') {
            next = make_pair(current.first == 0 ? gridSize - 1 : current.first - 1, current.second);
        }
        return next;
    }

    const deque<pair<int, int>>& body() const { return body_; }
    pair<int, int> head() const { return body_.back(); }

    bool contains(const pair<int, int>& cell) const {
        return find(body_.begin(), body_.end(), cell) != body_.end();
    }

    bool willCollideWithSelf(const pair<int, int>& nextHead) const {
        return contains(nextHead);
    }

    void growTo(const pair<int, int>& nextHead) {
        body_.push_back(nextHead);
    }

    void moveTo(const pair<int, int>& nextHead) {
        body_.push_back(nextHead);
        body_.pop_front();
    }

private:
    int gridSize_;
    deque<pair<int, int>> body_;
};

class Renderer {
public:
    void render(int size, const deque<pair<int, int>>& snake, const pair<int, int>& food) {
        cout << "Snake Game" << endl;
        for (size_t i = 0; i < static_cast<size_t>(size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(size); j++) {
                if (static_cast<int>(i) == food.first && static_cast<int>(j) == food.second) {
                    cout << "🍎";
                } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                    cout << "🐍";
                } else {
                    cout << "⬜";
                }
            }
            cout << endl;
        }
    }
};

class InputHandler {
public:
    InputHandler() : direction_('r') {}

    char direction() const { return direction_.load(); }

    void run() {
#ifdef _WIN32
        map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
        for (;;) {
            if (_kbhit()) {
                char input = _getch();
                if (keymap.find(input) != keymap.end()) {
                    direction_.store(keymap[input]);
                } else if (input == 'q') {
                    system("cls");
                    exit(0);
                }
            }
            sleep_for(10ms);
        }
#else
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
        while (true) {
            char input = getchar();
            if (keymap.find(input) != keymap.end()) {
                direction_.store(keymap[input]);
            } else if (input == 'q') {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                exit(0);
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    }

private:
    atomic<char> direction_;
};

class Game {
public:
    Game(int size = 8)
        : size_(size), snake_(size_), input_(), renderer_(), food_(make_pair(rand() % size_, rand() % size_)) {}

    void runInput() { input_.run(); }

    void runLoop() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        pair<int, int> head = make_pair(0, 1);
        for (;;) {
            cout << "\033[H";
            char dir = input_.direction();
            head = Snake::computeNextHead(head, dir, size_);

            if (snake_.willCollideWithSelf(head)) {
#ifdef _WIN32
                system("cls");
#else
                system("clear");
#endif
                cout << "Game Over" << endl;
                exit(0);
            } else if (head.first == food_.first && head.second == food_.second) {
                food_ = make_pair(rand() % size_, rand() % size_);
                snake_.growTo(head);
            } else {
                snake_.moveTo(head);
            }

            renderer_.render(size_, snake_.body(), food_);
            cout << "length of snake: " << snake_.body().size() << endl;
            sleep_for(500ms);
        }
    }

private:
    int size_;
    Snake snake_;
    InputHandler input_;
    Renderer renderer_;
    pair<int, int> food_;
};

inline pair<int,int> get_next_head(pair<int,int> current, char direction){
    return Snake::computeNextHead(current, direction, 10);
}

#endif // SNAKE_H


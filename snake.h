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
    explicit Snake(int boardSize)
        : boardSize_(boardSize) {
        bodySegments_.push_back(make_pair(0, 0));
    }

    static pair<int, int> computeNextHead(const pair<int, int>& currentHead, char moveDirection, int boardSize) {
        pair<int, int> nextHead = currentHead;
        if (moveDirection == 'r') {
            nextHead = make_pair(currentHead.first, (currentHead.second + 1) % boardSize);
        } else if (moveDirection == 'l') {
            nextHead = make_pair(currentHead.first, currentHead.second == 0 ? boardSize - 1 : currentHead.second - 1);
        } else if (moveDirection == 'd') {
            nextHead = make_pair((currentHead.first + 1) % boardSize, currentHead.second);
        } else if (moveDirection == 'u') {
            nextHead = make_pair(currentHead.first == 0 ? boardSize - 1 : currentHead.first - 1, currentHead.second);
        }
        return nextHead;
    }

    const deque<pair<int, int>>& body() const { return bodySegments_; }
    pair<int, int> head() const { return bodySegments_.back(); }

    bool contains(const pair<int, int>& cellPos) const {
        return find(bodySegments_.begin(), bodySegments_.end(), cellPos) != bodySegments_.end();
    }

    bool willCollideWithSelf(const pair<int, int>& nextHead) const {
        return contains(nextHead);
    }

    void growTo(const pair<int, int>& nextHead) {
        bodySegments_.push_back(nextHead);
    }

    void moveTo(const pair<int, int>& nextHead) {
        bodySegments_.push_back(nextHead);
        bodySegments_.pop_front();
    }

private:
    int boardSize_;
    deque<pair<int, int>> bodySegments_;
};

class Renderer {
public:
    void render(int boardSize, const deque<pair<int, int>>& snakeBody, const pair<int, int>& foodPos) {
        cout << "Snake Game" << endl;
        for (size_t row = 0; row < static_cast<size_t>(boardSize); row++) {
            for (size_t col = 0; col < static_cast<size_t>(boardSize); col++) {
                if (static_cast<int>(row) == foodPos.first && static_cast<int>(col) == foodPos.second) {
                    cout << "🍎";
                } else if (find(snakeBody.begin(), snakeBody.end(), make_pair(int(row), int(col))) != snakeBody.end()) {
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
    InputHandler() : currentDirection_('r') {}

    char direction() const { return currentDirection_.load(); }

    void run() {
#ifdef _WIN32
        map<char, char> keyMap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
        for (;;) {
            if (_kbhit()) {
                char userInput = _getch();
                if (keyMap.find(userInput) != keyMap.end()) {
                    currentDirection_.store(keyMap[userInput]);
                } else if (userInput == 'q') {
                    system("cls");
                    exit(0);
                }
            }
            sleep_for(10ms);
        }
#else
        struct termios oldSettings, newSettings;
        tcgetattr(STDIN_FILENO, &oldSettings);
        newSettings = oldSettings;
        newSettings.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
        map<char, char> keyMap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
        while (true) {
            char userInput = getchar();
            if (keyMap.find(userInput) != keyMap.end()) {
                currentDirection_.store(keyMap[userInput]);
            } else if (userInput == 'q') {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
                exit(0);
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
#endif
    }

private:
    atomic<char> currentDirection_;
};

class Game {
public:
    Game(int boardSize = 8)
        : boardSize_(boardSize),
          snake_(boardSize_),
          inputHandler_(),
          renderer_(),
          foodPos_(make_pair(rand() % boardSize_, rand() % boardSize_)) {}

    void runInput() { inputHandler_.run(); }

    void runLoop() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        pair<int, int> currentHead = make_pair(0, 1);
        for (;;) {
            cout << "\033[H";
            char moveDir = inputHandler_.direction();
            currentHead = Snake::computeNextHead(currentHead, moveDir, boardSize_);

            if (snake_.willCollideWithSelf(currentHead)) {
#ifdef _WIN32
                system("cls");
#else
                system("clear");
#endif
                cout << "Game Over" << endl;
                exit(0);
            } else if (currentHead.first == foodPos_.first && currentHead.second == foodPos_.second) {
                foodPos_ = make_pair(rand() % boardSize_, rand() % boardSize_);
                snake_.growTo(currentHead);
            } else {
                snake_.moveTo(currentHead);
            }

            renderer_.render(boardSize_, snake_.body(), foodPos_);
            cout << "Snake length: " << snake_.body().size() << endl;
            sleep_for(500ms);
        }
    }

private:
    int boardSize_;
    Snake snake_;
    InputHandler inputHandler_;
    Renderer renderer_;
    pair<int, int> foodPos_;
};

inline pair<int,int> get_next_head(pair<int,int> currentHead, char moveDirection){
    return Snake::computeNextHead(currentHead, moveDirection, 10);
}

#endif // SNAKE_H

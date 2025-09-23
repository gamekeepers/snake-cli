#pragma once
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
#include <fstream>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

const int BOARD_SIZE = 10;

// ================= SNAKE =================
class Snake {
private:
    deque<pair<int,int>> body;
    char direction;   // 'r','l','u','d'
public:
    Snake() {
        body.push_back({0,0});
        direction = 'r';
    }

    pair<int,int> head() const { return body.back(); }
    deque<pair<int,int>> getBody() const { return body; }
    char getDirection() const { return direction; }

    void setDirection(char d) { direction = d; }

    void move(pair<int,int> newHead, bool grow=false) {
        body.push_back(newHead);
        if (!grow) body.pop_front();
    }

    bool hasCollision(pair<int,int> pos) const {
        return find(body.begin(), body.end(), pos) != body.end();
    }
};

// ================= FOOD =================
class Food {
private:
    pair<int,int> food;
    pair<int,int> poison;
public:
    Food() {
        food = {-1,-1};
        poison = {-1,-1};
    }

    pair<int,int> getFood() const { return food; }
    pair<int,int> getPoison() const { return poison; }

    void spawn(const deque<pair<int,int>>& snake) {
        food = random_food(snake);
        poison = random_poison(snake, food);
    }

    static pair<int,int> random_food(const deque<pair<int,int>>& snake) {
        pair<int, int> pos;
        do {
            pos = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
        } while (find(snake.begin(), snake.end(), pos) != snake.end());
        return pos;
    }

    static pair<int, int> random_poison(const deque<pair<int, int>>& snake, pair<int, int> food) {
        pair<int, int> pos;
        do {
            pos = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
        } while (find(snake.begin(), snake.end(), pos) != snake.end() || pos == food);
        return pos;
    }
};

// ================= GAME =================
class Game {
private:
    Snake snake;
    Food food;
    bool paused;
    int score;
    vector<int> highScores;

public:
    Game() {
        paused = false;
        score = 0;
        load_scores();
        food.spawn(snake.getBody());
    }

    // ---------- Input ----------
    void input_handler() {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        map<char, char> keymap = {
            {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}
        };

        while (true) {
            char input = getchar();
            if (keymap.find(input) != keymap.end()) {
                snake.setDirection(keymap[input]);
            } else if (input == 'q') {
                exit(0);
            } else if (input == 'p') {
                paused = !paused;
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    // ---------- Rendering ----------
    void render() {
        system("clear");
        auto body = snake.getBody();
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (make_pair(i, j) == food.getFood()) cout << "🍎";
                else if (make_pair(i, j) == food.getPoison()) cout << "💀";
                else if (find(body.begin(), body.end(), make_pair(i,j)) != body.end()) cout << "🐍";
                else cout << "⬜";
            }
            cout << endl;
        }
        cout << "Score: " << score << endl;
    }

    // ---------- Mechanics ----------
    static pair<int,int> get_next_head(pair<int,int> current, char dir) {
        pair<int, int> next;
        if (dir == 'r') {
            next = make_pair(current.first, (current.second + 1) % BOARD_SIZE);
        } else if (dir == 'l') {
            next = make_pair(current.first, current.second == 0 ? BOARD_SIZE - 1 : current.second - 1);
        } else if (dir == 'd') {
            next = make_pair((current.first + 1) % BOARD_SIZE, current.second);
        } else if (dir == 'u') {
            next = make_pair(current.first == 0 ? BOARD_SIZE - 1 : current.first - 1, current.second);
        }
        return next;
    }

    void save_score() {
        highScores.push_back(score);
        sort(highScores.rbegin(), highScores.rend());
        if (highScores.size() > 10) highScores.pop_back();

        ofstream file("scores.txt");
        for (int sc : highScores) file << sc << endl;
        file.close();
    }

    void load_scores() {
        ifstream file("scores.txt");
        int s;
        while (file >> s) highScores.push_back(s);
        file.close();
        sort(highScores.rbegin(), highScores.rend());
        if (highScores.size() > 10) highScores.resize(10);
    }

    // ---------- Main Game ----------
    void play() {
        auto head = snake.head();
        while (true) {
            if (paused) {
                this_thread::sleep_for(200ms);
                continue;
            }
            cout << "\033[H";
            head = get_next_head(head, snake.getDirection());

            // Self collision
            if (snake.hasCollision(head)) {
                system("clear");
                cout << "Game Over (hit yourself)\nFinal Score: " << score << endl;
                save_score();
                exit(0);
            }

            // Eat normal food
            if (head == food.getFood()) {
                score += 10;
                food.spawn(snake.getBody());
                snake.move(head, true);
            }
            // Eat poison
            else if (head == food.getPoison()) {
                system("clear");
                cout << "Game Over (ate poison)\nFinal Score: " << score << endl;
                save_score();
                exit(0);
            }
            // Just move
            else {
                snake.move(head, false);
            }

            render();

            int speed = max(100, 500 - score * 10);
            sleep_for(milliseconds(speed));
        }
    }
};
